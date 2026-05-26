#ifndef SYSFAULT_TRANSLATION_H
#define SYSFAULT_TRANSLATION_H

#include "fault.h"

typedef struct {
    size_t id;
    const char *description;
    const char *common_causes;
    const char *possible_fixes;
} FaultTranslation;

extern const FaultTranslation en_fault_translations[];
extern const size_t en_fault_translations_count;
extern const FaultTranslation uk_fault_translations[];
extern const size_t uk_fault_translations_count;

const FaultTranslation *fault_translation_for_lang(const Fault *fault, language_t language);
const char *fault_description_for_lang(const Fault *fault, language_t language);
const char *fault_common_causes_for_lang(const Fault *fault, language_t language);
const char *fault_possible_fixes_for_lang(const Fault *fault, language_t language);

int validate_fault_translations(void);

const char *fault_translation_validation_error(void);

#endif
