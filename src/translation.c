#include "translation.h"

#include <stdio.h>

static char translation_error[160];

static const FaultTranslation *translation_from_table(const FaultTranslation *table, const fault_id_t id)
{
    if (id >= FAULT_ID_COUNT) {
        return NULL;
    }

    if (table[id].description == NULL) {
        return NULL;
    }

    return &table[id];
}

static const FaultTranslation *find_translation(fault_id_t id, fault_language_t language)
{
    switch (language) {
    case FAULT_LANG_EN:
        return translation_from_table(en_fault_translations, id);
    case FAULT_LANG_UK:
        return translation_from_table(uk_fault_translations, id);
    }

    return NULL;
}

const char *fault_description_for_lang(const Fault *fault, fault_language_t language)
{
    if (fault == NULL) {
        return "";
    }

    const FaultTranslation *translation = find_translation(fault->id, language);
    if (translation == NULL || translation->description == NULL) {
        return "";
    }

    return translation->description;
}

const char *fault_common_causes_for_lang(const Fault *fault, fault_language_t language)
{
    if (fault == NULL) {
        return "";
    }

    const FaultTranslation *translation = find_translation(fault->id, language);

    if (translation == NULL || translation->common_causes == NULL) {
        return "";
    }

    return translation->common_causes;
}

const char *fault_possible_fixes_for_lang(const Fault *fault, fault_language_t language)
{
    if (fault == NULL) {
        return "";
    }

    const FaultTranslation *translation = find_translation(fault->id, language);

    if (translation == NULL || translation->possible_fixes == NULL) {
        return "";
    }

    return translation->possible_fixes;
}

static int validate_language(const fault_language_t language, const char *language_name)
{
    for (size_t i = 0; i < fault_catalog_count; i++) {
        const FaultTranslation *translation = find_translation(fault_catalog[i].id, language);

        if (translation == NULL) {
            snprintf(translation_error, sizeof(translation_error),
                     "missing %s translation for %s",
                     language_name, fault_catalog[i].name);
            return 0;
        }

        if (translation->description == NULL ||
            translation->common_causes == NULL ||
            translation->possible_fixes == NULL) {
            snprintf(translation_error, sizeof(translation_error),
                     "incomplete %s translation for %s",
                     language_name, fault_catalog[i].name);
            return 0;
        }
    }

    return 1;
}

int validate_fault_translations(void)
{
    translation_error[0] = '\0';

    if (!validate_language(FAULT_LANG_EN, "en")) {
        return 0;
    }

    if (!validate_language(FAULT_LANG_UK, "uk")) {
        return 0;
    }

    return 1;
}

const char *fault_translation_validation_error(void)
{
    return translation_error;
}
