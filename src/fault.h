#ifndef SYSFAULT_FAULT_H
#define SYSFAULT_FAULT_H

#include <stddef.h>

#include "fault_ids.h"

typedef enum {
    FAULT_OS_LINUX,
    FAULT_OS_MACOS,
    FAULT_OS_WINDOWS,
    FAULT_OS_UNIX
} fault_os_t;

typedef enum {
    FAULT_KIND_ERRNO,
    FAULT_KIND_SIGNAL,
    FAULT_KIND_EXIT
} fault_kind_t;

typedef enum {
    FAULT_LANG_EN,
    FAULT_LANG_UK
} fault_language_t;

typedef struct {
    fault_id_t id;
    fault_os_t os;
    fault_kind_t kind;
    const char *name;
    const char *alias;
    int code;
    int has_code;
} Fault;

extern const Fault fault_catalog[];
extern const size_t fault_catalog_count;

void print_fault(const Fault *fault, fault_language_t language);
void print_list(fault_language_t language);
void print_usage(const char *progname, fault_language_t language);
void print_exit_status_hint(int code, fault_language_t language);

int validate_fault_catalog(void);

const char *fault_catalog_validation_error(void);

#endif
