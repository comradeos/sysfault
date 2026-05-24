#include "fault.h"

#include <stdio.h>

#include "translation.h"

static char fault_catalog_error[160];

static const char *kind_title(const fault_kind_t kind, const fault_language_t language)
{
    if (language == FAULT_LANG_UK) {
        switch (kind) {
        case FAULT_KIND_ERRNO:
            return "Linux errno";
        case FAULT_KIND_SIGNAL:
            return "Linux сигнали";
        case FAULT_KIND_EXIT:
            return "Unix коди завершення";
        }
    }

    switch (kind) {
    case FAULT_KIND_ERRNO:
        return "Linux errno";
    case FAULT_KIND_SIGNAL:
        return "Linux signals";
    case FAULT_KIND_EXIT:
        return "Unix exit codes";
    }

    return "Faults";
}

static const char *kind_category(const fault_kind_t kind, const fault_language_t language)
{
    if (language == FAULT_LANG_UK) {
        switch (kind) {
        case FAULT_KIND_ERRNO:
            return "errno";
        case FAULT_KIND_SIGNAL:
            return "сигнал";
        case FAULT_KIND_EXIT:
            return "код завершення";
        }
    }

    switch (kind) {
    case FAULT_KIND_ERRNO:
        return "errno";
    case FAULT_KIND_SIGNAL:
        return "signal";
    case FAULT_KIND_EXIT:
        return "exit code";
    }

    return "fault";
}

static int validate_catalog_ids(unsigned char *seen)
{
    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (fault_catalog[i].id < 0 || fault_catalog[i].id >= FAULT_ID_COUNT) {
            snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                     "entry %s has out-of-range id %d",
                     fault_catalog[i].name, (int) fault_catalog[i].id);
            return 0;
        }

        if (seen[fault_catalog[i].id] != 0) {
            snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                     "entry %s reuses id %d",
                     fault_catalog[i].name, (int) fault_catalog[i].id);
            return 0;
        }

        seen[fault_catalog[i].id] = 1;
    }

    return 1;
}

int validate_fault_catalog(void)
{
    unsigned char seen[FAULT_ID_COUNT];
    size_t i;

    for (i = 0; i < FAULT_ID_COUNT; i++) {
        seen[i] = 0;
    }

    fault_catalog_error[0] = '\0';

    if (!validate_catalog_ids(seen)) {
        return 0;
    }

    for (i = 0; i < FAULT_ID_COUNT; i++) {
        if (seen[i] == 0) {
            snprintf(fault_catalog_error, sizeof(fault_catalog_error),
                     "missing fault entry for id %d",
                     (int) i);
            return 0;
        }
    }

    return 1;
}

const char *fault_catalog_validation_error(void)
{
    return fault_catalog_error;
}

static void print_related(const Fault *fault, const fault_language_t language)
{
    int shown = 0;

    printf("%s\n", language == FAULT_LANG_UK ? "Схожі:" : "Related:");

    for (size_t i = 0; i < fault_catalog_count && shown < 3; i++) {
        if (&fault_catalog[i] == fault) {
            continue;
        }

        if (fault_catalog[i].kind != fault->kind) {
            continue;
        }

        if (fault_catalog[i].id == fault->id) {
            continue;
        }

        if (fault->kind == FAULT_KIND_ERRNO &&
            fault_catalog[i].has_code &&
            fault->has_code &&
            fault_catalog[i].code / 10 == fault->code / 10) {
            printf("- %s (%d): %s\n",
                   fault_catalog[i].name,
                   fault_catalog[i].code,
                   fault_description_for_lang(&fault_catalog[i], language));
            shown++;
            continue;
        }

        if (fault->kind == FAULT_KIND_SIGNAL &&
            fault_catalog[i].has_code &&
            fault->has_code &&
            ((fault->code <= 15 && fault_catalog[i].code <= 15) ||
             (fault->code >= 17 && fault_catalog[i].code >= 17 && fault_catalog[i].code <= 31))) {
            printf("- %s (%d): %s\n",
                   fault_catalog[i].name,
                   fault_catalog[i].code,
                   fault_description_for_lang(&fault_catalog[i], language));
            shown++;
        }
    }

    if (shown == 0) {
        printf("- %s\n",
               language == FAULT_LANG_UK ? "немає в поточному каталозі" : "none in the current catalog");
    }
}

void print_fault(const Fault *fault, fault_language_t language)
{
    if (fault == NULL) {
        return;
    }

    printf("%s", fault->name);
    if (fault->has_code) {
        printf(" (%d)", fault->code);
    }
    printf("\n");

    printf("%s %s\n\n",
           language == FAULT_LANG_UK ? "Категорія:" : "Category:",
           kind_category(fault->kind, language));
    printf("%s\n%s\n\n",
           language == FAULT_LANG_UK ? "Значення:" : "Meaning:",
           fault_description_for_lang(fault, language));
    printf("%s\n%s\n\n",
           language == FAULT_LANG_UK ? "Типові причини:" : "Common causes:",
           fault_common_causes_for_lang(fault, language));
    printf("%s\n%s\n\n",
           language == FAULT_LANG_UK ? "Можливі дії:" : "Possible fixes:",
           fault_possible_fixes_for_lang(fault, language));

    print_related(fault, language);
}

static void print_kind_list(const fault_kind_t kind, const fault_language_t language)
{
    printf("%s\n", kind_title(kind, language));

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (fault_catalog[i].kind != kind) {
            continue;
        }

        printf("  %-18s %3d  %s\n",
               fault_catalog[i].name,
               fault_catalog[i].code,
               fault_description_for_lang(&fault_catalog[i], language));
    }
    printf("\n");
}

void print_list(fault_language_t language)
{
    print_kind_list(FAULT_KIND_ERRNO, language);
    print_kind_list(FAULT_KIND_SIGNAL, language);
    print_kind_list(FAULT_KIND_EXIT, language);
}

void print_usage(const char *progname, fault_language_t language)
{
    if (language == FAULT_LANG_UK) {
        printf("Використання: %s [--lang en|uk] <назва|число|--list>\n\n", progname);
        printf("Приклади:\n");
        printf("  %s EACCES\n", progname);
        printf("  %s 13\n", progname);
        printf("  %s SIGSEGV\n", progname);
        printf("  %s 139\n", progname);
        printf("  %s --lang uk EACCES\n", progname);
        printf("  %s --lang uk --list\n", progname);
        return;
    }

    printf("Usage: %s [--lang en|uk] <name|number|--list>\n\n", progname);
    printf("Examples:\n");
    printf("  %s EACCES\n", progname);
    printf("  %s 13\n", progname);
    printf("  %s SIGSEGV\n", progname);
    printf("  %s 139\n", progname);
    printf("  %s --lang uk EACCES\n", progname);
    printf("  %s --lang uk --list\n", progname);
}

void print_exit_status_hint(int code, fault_language_t language)
{
    if (language == FAULT_LANG_UK) {
        if (code == 0) {
            printf("Пояснення коду завершення:\n0 означає успіх.\n");
            return;
        }

        if (code >= 1 && code <= 125) {
            printf("Пояснення коду завершення:\n%d є ненульовим кодом помилки, який визначає сама програма.\n", code);
            return;
        }

        if (code == 126) {
            printf("Пояснення коду завершення:\n126 зазвичай означає, що команду знайдено, але її не вдалося виконати.\n");
            return;
        }

        if (code == 127) {
            printf("Пояснення коду завершення:\n127 зазвичай означає, що команду не знайдено.\n");
            return;
        }

        if (code >= 128) {
            printf("Пояснення коду завершення:\n%d може кодувати сигнал як 128 + номер сигналу.\n", code);
            return;
        }

        printf("Пояснення коду завершення:\n%d є специфічним для програми.\n", code);
        return;
    }

    if (code == 0) {
        printf("Exit status hint:\n0 means success.\n");
        return;
    }

    if (code >= 1 && code <= 125) {
        printf("Exit status hint:\n%d is a non-zero application-defined exit status.\n", code);
        return;
    }

    if (code == 126) {
        printf("Exit status hint:\n126 usually means the command was found but could not be executed.\n");
        return;
    }

    if (code == 127) {
        printf("Exit status hint:\n127 usually means command not found.\n");
        return;
    }

    if (code >= 128) {
        printf("Exit status hint:\n%d may encode a signal as 128 + signal number.\n", code);
        return;
    }

    printf("Exit status hint:\n%d is application-specific.\n", code);
}
