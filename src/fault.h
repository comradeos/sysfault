#ifndef SYSFAULT_FAULT_H
#define SYSFAULT_FAULT_H

#include <stddef.h>

#define FAULT_CATALOG_SIZE 206

typedef enum {
    LINUX,
    MACOS,
    WINDOWS
} os_t;

typedef enum {
    ERRNO,
    SIGNAL,
    EXIT
} kind_t;

typedef enum {
    EN,
    UK
} language_t;

typedef struct {
    size_t id;
    os_t os;
    kind_t kind;
    const char *name;
    const char *alias;
    int code;
    int has_code;
} Fault;

extern const Fault fault_catalog[];
extern const size_t fault_catalog_count;

os_t detect_current_os(void);
int fault_matches_os(const Fault *fault, os_t selected_os);
size_t fault_display_id(const Fault *fault);
const char *fault_os_name(os_t os);
const char *fault_kind_name(kind_t kind, language_t language);

void print_fault_summary(const Fault *fault, language_t language);
void print_fault_details(const Fault *fault, language_t language);
void print_fault_results(const Fault *const *faults, size_t count, language_t language);
void print_list(os_t selected_os, language_t language);
void print_usage(const char *progname, os_t detected_os);

int validate_fault_catalog(void);

const char *fault_catalog_validation_error(void);

#endif
