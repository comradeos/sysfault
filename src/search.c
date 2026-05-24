#include "search.h"

#include <ctype.h>
#include <stdlib.h>

static int char_equal(int left, int right)
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

static const Fault *find_by_name(const Fault *faults, size_t count, const char *name)
{
    size_t i;

    for (i = 0; i < count; i++) {
        if (string_equal(faults[i].name, name)) {
            return &faults[i];
        }

        if (faults[i].alias != NULL && string_equal(faults[i].alias, name)) {
            return &faults[i];
        }
    }

    return NULL;
}

static const Fault *find_by_code(const Fault *faults, size_t count, int code)
{
    size_t i;

    for (i = 0; i < count; i++) {
        if (faults[i].code == code) {
            return &faults[i];
        }
    }

    return NULL;
}

int parse_code(const char *text, int *code)
{
    char *end;
    long value;

    if (text == NULL || *text == '\0') {
        return 0;
    }

    value = strtol(text, &end, 10);
    if (*end != '\0' || value < 0 || value > 255) {
        return 0;
    }

    *code = (int) value;
    return 1;
}

const Fault *find_errno_by_name(const char *name)
{
    return find_by_name(errno_faults, errno_faults_count, name);
}

const Fault *find_errno_by_code(int code)
{
    return find_by_code(errno_faults, errno_faults_count, code);
}

const Fault *find_signal_by_name(const char *name)
{
    return find_by_name(signal_faults, signal_faults_count, name);
}

const Fault *find_signal_by_code(int code)
{
    return find_by_code(signal_faults, signal_faults_count, code);
}

const Fault *find_exit_by_name(const char *name)
{
    return find_by_name(exit_faults, exit_faults_count, name);
}

const Fault *find_exit_by_code(int code)
{
    return find_by_code(exit_faults, exit_faults_count, code);
}

const Fault *find_any_by_name(const char *name)
{
    const Fault *fault;

    fault = find_errno_by_name(name);
    if (fault != NULL) {
        return fault;
    }

    fault = find_signal_by_name(name);
    if (fault != NULL) {
        return fault;
    }

    return find_exit_by_name(name);
}

const Fault *decode_exit_signal(int exit_code)
{
    if (exit_code < 129) {
        return NULL;
    }

    return find_signal_by_code(exit_code - 128);
}

size_t collect_faults_by_code(int code, const Fault **matches, size_t capacity)
{
    const Fault *fault;
    size_t count;

    count = 0;

    fault = find_errno_by_code(code);
    if (fault != NULL && count < capacity) {
        matches[count++] = fault;
    }

    fault = find_signal_by_code(code);
    if (fault != NULL && count < capacity) {
        matches[count++] = fault;
    }

    fault = find_exit_by_code(code);
    if (fault != NULL && count < capacity) {
        matches[count++] = fault;
    }

    return count;
}
