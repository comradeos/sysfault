#include "fault.h"

#include <stdio.h>

#include "translation.h"

static char fault_catalog_error[160];

os_t detect_current_os(void)
{
#if defined(_WIN32)
    return WINDOWS;
#elif defined(__APPLE__)
    return MACOS;
#else
    return LINUX;
#endif
}

int fault_matches_os(const Fault *fault, const os_t selected_os)
{
    if (fault == NULL) {
        return 0;
    }

    return fault->os == selected_os;
}

size_t fault_display_id(const Fault *fault)
{
    if (fault == NULL) {
        return 0;
    }

    return fault->id;
}

const char *fault_os_name(const os_t os)
{
    switch (os) {
    case LINUX:
        return "linux";
    case MACOS:
        return "macos";
    case WINDOWS:
        return "windows";
    }

    return "unknown";
}

const char *fault_kind_name(const kind_t kind, const language_t language)
{
    if (language == UK) {
        switch (kind) {
        case ERRNO:
            return "помилка";
        case SIGNAL:
            return "сигнал";
        case EXIT:
            return "вихід";
        }
    }

    switch (kind) {
    case ERRNO:
        return "error";
    case SIGNAL:
        return "signal";
    case EXIT:
        return "exit";
    }

    return "event";
}

int validate_fault_catalog(void)
{
    unsigned char seen[FAULT_CATALOG_SIZE + 1];

    for (size_t i = 0; i <= FAULT_CATALOG_SIZE; i++) {
        seen[i] = 0;
    }

    if (fault_catalog_count != FAULT_CATALOG_SIZE) {
        snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                 "catalog count %zu does not match expected size %d",
                 fault_catalog_count, FAULT_CATALOG_SIZE);
        return 0;
    }

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (fault_catalog[i].id == 0 || fault_catalog[i].id > FAULT_CATALOG_SIZE) {
            snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                     "entry %s has out-of-range id %zu",
                     fault_catalog[i].name, fault_catalog[i].id);
            return 0;
        }

        if (seen[fault_catalog[i].id] != 0) {
            snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                     "entry %s reuses id %zu",
                     fault_catalog[i].name, fault_catalog[i].id);
            return 0;
        }

        seen[fault_catalog[i].id] = 1;
    }

    for (size_t i = 1; i <= FAULT_CATALOG_SIZE; i++) {
        if (seen[i] == 0) {
            snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                     "missing fault entry for id %zu",
                     i);
            return 0;
        }
    }

    return 1;
}

const char *fault_catalog_validation_error(void)
{
    return fault_catalog_error;
}

void print_fault_summary(const Fault *fault, const language_t language)
{
    if (fault == NULL) {
        return;
    }

    printf("%zu: %s, %s, %s",
           fault_display_id(fault),
           fault_os_name(fault->os),
           fault_kind_name(fault->kind, language),
           fault->name != NULL ? fault->name : "-");

    if (fault->has_code) {
        printf(" (%d)", fault->code);
    }

    printf("\n");
}

void print_fault_details(const Fault *fault, const language_t language)
{
    const FaultTranslation *translation;

    if (fault == NULL) {
        return;
    }

    translation = fault_translation_for_lang(fault, language);

    print_fault_summary(fault, language);
    printf("\n");

    printf("%s %s\n",
           language == UK ? "ОС:" : "OS:",
           fault_os_name(fault->os));
    printf("%s %s\n",
           language == UK ? "Тип:" : "Type:",
           fault_kind_name(fault->kind, language));

    if (fault->has_code) {
        printf("%s %d\n",
               language == UK ? "Системний код:" : "System code:",
               fault->code);
    }

    printf("%s %s\n",
           language == UK ? "Системне ім'я:" : "System name:",
           fault->name != NULL ? fault->name : "-");

    if (fault->alias != NULL) {
        printf("%s %s\n",
               language == UK ? "Аліас:" : "Alias:",
               fault->alias);
    }

    if (translation == NULL) {
        return;
    }

    printf("\n%s\n%s\n",
           language == UK ? "Опис:" : "Description:",
           translation->description != NULL ? translation->description : "");
    printf("\n%s\n%s\n",
           language == UK ? "Причини:" : "Common causes:",
           translation->common_causes != NULL ? translation->common_causes : "");
    printf("\n%s\n%s\n",
           language == UK ? "Можливі рішення:" : "Possible fixes:",
           translation->possible_fixes != NULL ? translation->possible_fixes : "");
}

void print_fault_results(const Fault *const *faults, const size_t count, const language_t language)
{
    if (count == 0) {
        printf("%s\n", language == UK ? "Нічого не знайдено." : "No matches found.");
        return;
    }

    if (count == 1) {
        print_fault_details(faults[0], language);
        return;
    }

    for (size_t i = 0; i < count; i++) {
        print_fault_summary(faults[i], language);
    }
}

void print_list(const os_t selected_os, const language_t language)
{
    size_t shown = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (!fault_matches_os(&fault_catalog[i], selected_os)) {
            continue;
        }

        print_fault_summary(&fault_catalog[i], language);
        shown++;
    }

    if (shown == 0) {
        printf("%s\n",
               language == UK ? "Для вибраної ОС записів немає." : "No catalog entries for the selected OS.");
    }
}

void print_usage(const char *progname, const os_t detected_os)
{
    printf("Usage: %s [-en|-ua] [-linux|-macos|-windows] <code|name>\n", progname);
    printf("       %s [-en|-ua] [-linux|-macos|-windows] -f <text>\n", progname);
    printf("       %s [-en|-ua] [-linux|-macos|-windows] -id <number>\n", progname);
    printf("       %s [-en|-ua] [-linux|-macos|-windows] --list\n\n", progname);
    printf("Current OS filter by default: %s\n\n", fault_os_name(detected_os));
    printf("Examples:\n");
    printf("  %s 13\n", progname);
    printf("  %s 13 -linux\n", progname);
    printf("  %s SIGSEGV\n", progname);
    printf("  %s -f segmentation\n", progname);
    printf("  %s -id 25\n", progname);
    printf("  %s -ua -f \"сегмента\"\n", progname);
    printf("  %s --list\n", progname);
}
