#ifndef SYSFAULT_FAULT_H
#define SYSFAULT_FAULT_H

#include <stddef.h>

typedef enum {
    FAULT_KIND_ERRNO,
    FAULT_KIND_SIGNAL,
    FAULT_KIND_EXIT
} fault_kind_t;

typedef struct {
    const char *name;
    const char *alias;
    int code;
    const char *category;
    const char *description;
    const char *common_causes;
    const char *possible_fixes;
    fault_kind_t kind;
} Fault;

extern const Fault errno_faults[];
extern const size_t errno_faults_count;

extern const Fault signal_faults[];
extern const size_t signal_faults_count;

extern const Fault exit_faults[];
extern const size_t exit_faults_count;

void print_fault(const Fault *fault);
void print_fault_list(const char *title, const Fault *faults, size_t count);
void print_list(void);
void print_usage(const char *progname);
void print_exit_status_hint(int code);

#endif
