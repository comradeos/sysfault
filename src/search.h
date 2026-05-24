#ifndef SYSFAULT_SEARCH_H
#define SYSFAULT_SEARCH_H

#include <stddef.h>

#include "fault.h"

int parse_code(const char *text, int *code);

const Fault *find_errno_by_name(const char *name);
const Fault *find_errno_by_code(int code);

const Fault *find_signal_by_name(const char *name);
const Fault *find_signal_by_code(int code);

const Fault *find_exit_by_name(const char *name);
const Fault *find_exit_by_code(int code);

const Fault *find_any_by_name(const char *name);
const Fault *decode_exit_signal(int exit_code);
size_t collect_faults_by_code(int code, const Fault **matches, size_t capacity);

#endif
