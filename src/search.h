#ifndef SYSFAULT_SEARCH_H
#define SYSFAULT_SEARCH_H

#include "fault.h"

int parse_code(const char *text, int *code);
int parse_fault_id(const char *text, size_t *id);

size_t collect_faults_by_code(int code, os_t selected_os, const Fault **matches, size_t capacity);
size_t collect_faults_by_name(const char *query, os_t selected_os, const Fault **matches, size_t capacity);
size_t collect_faults_by_description(const char *query, os_t selected_os, language_t language,
                                     const Fault **matches, size_t capacity);
size_t collect_faults_by_id(size_t display_id, os_t selected_os, const Fault **matches, size_t capacity);
size_t collect_faults_for_os(os_t selected_os, const Fault **matches, size_t capacity);

#endif
