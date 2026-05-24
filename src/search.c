#include "search.h"

#include <ctype.h>
#include <stdlib.h>

static int char_equal(const int left, const int right)
{
    return tolower((unsigned char) left) == tolower((unsigned char) right);
}

static int string_equal(const char *left, const char *right)
{
    while (*left != '\0' && *right != '\0') {
        if (!char_equal(*left, *right)) {
            return 0;
        }
        left++;
        right++;
    }

    return *left == '\0' && *right == '\0';
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

const Fault *find_any_by_name(const char *name)
{
    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (string_equal(fault_catalog[i].name, name)) {
            return &fault_catalog[i];
        }

        if (fault_catalog[i].alias != NULL && string_equal(fault_catalog[i].alias, name)) {
            return &fault_catalog[i];
        }
    }

    return NULL;
}

const Fault *decode_exit_signal(const int exit_code)
{
    if (exit_code < 129) {
        return NULL;
    }

    const int signal_code = exit_code - 128;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (fault_catalog[i].kind == FAULT_KIND_SIGNAL &&
            fault_catalog[i].has_code &&
            fault_catalog[i].code == signal_code) {
            return &fault_catalog[i];
        }
    }

    return NULL;
}

size_t collect_faults_by_code(const int code, const Fault **matches, const size_t capacity)
{
    size_t count = 0;

    for (size_t i = 0; i < fault_catalog_count; i++) {
        if (!fault_catalog[i].has_code || fault_catalog[i].code != code) {
            continue;
        }

        if (count < capacity) {
            matches[count++] = &fault_catalog[i];
        }
    }

    return count;
}
