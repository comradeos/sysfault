#include "translation.h"

#include <stdio.h>

static char translation_error[160];

static const FaultTranslation *translation_from_table(const FaultTranslation *table, const size_t table_count,
                                                      const size_t id)
{
    for (size_t i = 0; i < table_count; i++) {
        if (table[i].id == id) {
            return &table[i];
        }
    }

    return NULL;
}

static const FaultTranslation *find_translation(const size_t id, const language_t language)
{
    switch (language) {
    case EN:
        return translation_from_table(en_fault_translations, en_fault_translations_count, id);
    case UK:
        return translation_from_table(uk_fault_translations, uk_fault_translations_count, id);
    }

    return NULL;
}

const FaultTranslation *fault_translation_for_lang(const Fault *fault, const language_t language)
{
    if (fault == NULL) {
        return NULL;
    }

    return find_translation(fault->id, language);
}

const char *fault_description_for_lang(const Fault *fault, const language_t language)
{
    const FaultTranslation *translation = fault_translation_for_lang(fault, language);

    if (translation == NULL || translation->description == NULL) {
        return "";
    }

    return translation->description;
}

const char *fault_common_causes_for_lang(const Fault *fault, const language_t language)
{
    const FaultTranslation *translation = fault_translation_for_lang(fault, language);

    if (translation == NULL || translation->common_causes == NULL) {
        return "";
    }

    return translation->common_causes;
}

const char *fault_possible_fixes_for_lang(const Fault *fault, const language_t language)
{
    const FaultTranslation *translation = fault_translation_for_lang(fault, language);

    if (translation == NULL || translation->possible_fixes == NULL) {
        return "";
    }

    return translation->possible_fixes;
}

static int validate_translation_ids(const FaultTranslation *table, const size_t table_count,
                                    const char *language_name, unsigned char *seen)
{
    for (size_t i = 0; i < table_count; i++) {
        if (table[i].id == 0 || table[i].id > FAULT_CATALOG_SIZE) {
            snprintf(translation_error, sizeof(translation_error),
                     "out-of-range %s translation id %zu",
                     language_name, table[i].id);
            return 0;
        }

        if (seen[table[i].id] != 0) {
            snprintf(translation_error, sizeof(translation_error),
                     "duplicate %s translation id %zu",
                     language_name, table[i].id);
            return 0;
        }

        if (table[i].description == NULL || table[i].common_causes == NULL || table[i].possible_fixes == NULL) {
            snprintf(translation_error, sizeof(translation_error),
                     "incomplete %s translation for id %zu",
                     language_name, table[i].id);
            return 0;
        }

        seen[table[i].id] = 1;
    }

    return 1;
}

static int validate_language(const language_t language, const char *language_name)
{
    unsigned char seen[FAULT_CATALOG_SIZE + 1];
    const FaultTranslation *table = NULL;
    size_t table_count = 0;

    for (size_t i = 0; i <= FAULT_CATALOG_SIZE; i++) {
        seen[i] = 0;
    }

    switch (language) {
    case EN:
        table = en_fault_translations;
        table_count = en_fault_translations_count;
        break;
    case UK:
        table = uk_fault_translations;
        table_count = uk_fault_translations_count;
        break;
    }

    if (table_count != fault_catalog_count) {
        snprintf(translation_error, sizeof(translation_error),
                 "%s translation count %zu does not match catalog count %zu",
                 language_name, table_count, fault_catalog_count);
        return 0;
    }

    if (!validate_translation_ids(table, table_count, language_name, seen)) {
        return 0;
    }

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (find_translation(fault_catalog[i].id, language) == NULL) {
            snprintf(translation_error, sizeof(translation_error),
                     "missing %s translation for %s",
                     language_name, fault_catalog[i].name);
            return 0;
        }
    }

    return 1;
}

int validate_fault_translations(void)
{
    translation_error[0] = '\0';

    if (!validate_language(EN, "en")) {
        return 0;
    }

    if (!validate_language(UK, "uk")) {
        return 0;
    }

    return 1;
}

const char *fault_translation_validation_error(void)
{
    return translation_error;
}
