#ifndef SYSFAULT_TRANSLATION_H
#define SYSFAULT_TRANSLATION_H

#include "fault.h"

typedef struct {
    const char *description;
    const char *common_causes;
    const char *possible_fixes;
} FaultTranslation;

extern const FaultTranslation en_fault_translations[FAULT_ID_COUNT];
extern const FaultTranslation uk_fault_translations[FAULT_ID_COUNT];

const char *fault_description_for_lang(const Fault *fault, fault_language_t language);
const char *fault_common_causes_for_lang(const Fault *fault, fault_language_t language);
const char *fault_possible_fixes_for_lang(const Fault *fault, fault_language_t language);

int validate_fault_translations(void);

const char *fault_translation_validation_error(void);

#endif
