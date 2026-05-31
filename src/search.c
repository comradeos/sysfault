#include "search.h"

#include <ctype.h>
#include <stdlib.h>

#include "translation.h"

static int char_equal(const int left, const int right)
{
    return tolower((unsigned char) left) == tolower((unsigned char) right);
}

static int string_contains(const char *haystack, const char *needle)
{
    if (needle == NULL || *needle == '\0') {
        return 1;
    }

    if (haystack == NULL || *haystack == '\0') {
        return 0;
    }

    for (const char *cursor = haystack; *cursor != '\0'; cursor++) {
        const char *left = cursor;
        const char *right = needle;

        while (*left != '\0' && *right != '\0' && char_equal(*left, *right)) {
            left++;
            right++;
        }

        if (*right == '\0') {
            return 1;
        }
    }

    return 0;
}

static size_t append_match(const Fault *fault, const Fault **matches, const size_t capacity, size_t count)
{
    if (count < capacity) {
        matches[count] = fault;
    }

    return count + 1;
}

int parse_code(const char *text, int *code)
{
    char *end;

    if (text == NULL || *text == '\0') {
        return 0;
    }

    const long value = strtol(text, &end, 10);

    if (*end != '\0' || value < 0 || value > 255) {
        return 0;
    }

    *code = (int) value;
    return 1;
}

int parse_fault_id(const char *text, size_t *id)
{
    char *end;

    if (text == NULL || *text == '\0') {
        return 0;
    }

    const unsigned long value = strtoul(text, &end, 10);

    if (*end != '\0' || value == 0 || value > FAULT_CATALOG_SIZE) {
        return 0;
    }

    *id = (size_t) value;
    return 1;
}

size_t collect_faults_by_code(const int code, const os_t selected_os, const Fault **matches, const size_t capacity)
{
    size_t count = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (!fault_matches_os(&fault_catalog[i], selected_os)) {
            continue;
        }

        if (!fault_catalog[i].has_code || fault_catalog[i].code != code) {
            continue;
        }

        count = append_match(&fault_catalog[i], matches, capacity, count);
    }

    return count;
}

size_t collect_faults_by_name(const char *query, const os_t selected_os, const Fault **matches,
                              const size_t capacity)
{
    size_t count = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (!fault_matches_os(&fault_catalog[i], selected_os)) {
            continue;
        }

        if (string_contains(fault_catalog[i].name, query) ||
            (fault_catalog[i].alias != NULL && string_contains(fault_catalog[i].alias, query))) {
            count = append_match(&fault_catalog[i], matches, capacity, count);
        }
    }

    return count;
}

size_t collect_faults_by_description(const char *query, const os_t selected_os,
                                     const language_t language, const Fault **matches, const size_t capacity)
{
    size_t count = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        const Fault *fault = &fault_catalog[i];
        const FaultTranslation *translation;

        if (!fault_matches_os(fault, selected_os)) {
            continue;
        }

        translation = fault_translation_for_lang(fault, language);
        if (translation == NULL) {
            continue;
        }

        if (string_contains(fault->name, query) ||
            (fault->alias != NULL && string_contains(fault->alias, query)) ||
            string_contains(translation->description, query) ||
            string_contains(translation->common_causes, query) ||
            string_contains(translation->possible_fixes, query)) {
            count = append_match(fault, matches, capacity, count);
        }
    }

    return count;
}

size_t collect_faults_by_id(const size_t display_id, const os_t selected_os, const Fault **matches,
                            const size_t capacity)
{
    (void) selected_os;

    size_t count = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (fault_display_id(&fault_catalog[i]) == display_id) {
            count = append_match(&fault_catalog[i], matches, capacity, count);
        }
    }

    return count;
}

size_t collect_faults_for_os(const os_t selected_os, const Fault **matches, const size_t capacity)
{
    size_t count = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (!fault_matches_os(&fault_catalog[i], selected_os)) {
            continue;
        }

        count = append_match(&fault_catalog[i], matches, capacity, count);
    }

    return count;
}
