#include "fault.h"

#include <stdio.h>

#define ARRAY_LEN(items) (sizeof(items) / sizeof((items)[0]))

const Fault exit_faults[] = {
    {"EXIT_SUCCESS", "SUCCESS", 0, "exit", "Success", "The program reported normal completion.", "No action needed.", FAULT_KIND_EXIT},
    {"EXIT_FAILURE", "FAILURE", 1, "exit", "Generic failure", "The program returned a non-specific failure code.", "Inspect program logs, stderr output, and surrounding context.", FAULT_KIND_EXIT},
    {"EXIT_BUILTIN_MISUSE", "MISUSE", 2, "exit", "Shell misuse or invalid builtin usage", "Shells often use 2 for command syntax errors or builtin misuse.", "Review command syntax, quoting, and builtin arguments.", FAULT_KIND_EXIT},
    {"EXIT_CANNOT_EXEC", "CANNOT_EXEC", 126, "exit", "Command found but not executable", "The file exists, but permissions, format, or exec policy blocked execution.", "Check executable bit, shebang, architecture, and policy restrictions.", FAULT_KIND_EXIT},
    {"EXIT_NOT_FOUND", "NOT_FOUND", 127, "exit", "Command not found", "The shell could not resolve the command in PATH.", "Check the command name, PATH, and installation status.", FAULT_KIND_EXIT},
    {"EXIT_INVALID_ARGUMENT", "INVALID_ARGUMENT", 128, "exit", "Invalid exit argument", "Shells often use 128 when exit gets an invalid numeric argument.", "Pass a valid numeric exit status in the 0-255 range.", FAULT_KIND_EXIT},
    {"EXIT_SIGINT", "SIGINT_EXIT", 130, "exit", "Interrupted by SIGINT", "The process usually ended after Ctrl-C or another SIGINT delivery.", "Review who sent SIGINT and whether cleanup handlers ran.", FAULT_KIND_EXIT},
    {"EXIT_SIGQUIT", "SIGQUIT_EXIT", 131, "exit", "Quit by SIGQUIT", "The process terminated because signal 3 was delivered.", "Check crash context, debugger usage, and generated core files.", FAULT_KIND_EXIT},
    {"EXIT_SIGABRT", "SIGABRT_EXIT", 134, "exit", "Aborted by SIGABRT", "The process called abort() or hit a fatal runtime assertion.", "Read assertions, logs, and panic messages leading to abort.", FAULT_KIND_EXIT},
    {"EXIT_SIGKILL", "SIGKILL_EXIT", 137, "exit", "Killed by SIGKILL", "The process was forcibly terminated, often by the kernel or an administrator.", "Check OOM killer logs, service managers, and kill events.", FAULT_KIND_EXIT},
    {"EXIT_SIGSEGV", "SIGSEGV_EXIT", 139, "exit", "Segmentation fault exit", "Shells encode fatal signals as 128 + signal, so 139 usually means SIGSEGV.", "Debug memory safety issues with logs, sanitizers, or a debugger.", FAULT_KIND_EXIT},
    {"EXIT_SIGTERM", "SIGTERM_EXIT", 143, "exit", "Terminated by SIGTERM", "A supervisor, init system, or user requested a clean shutdown.", "Check service restarts, deployments, and termination hooks.", FAULT_KIND_EXIT},
    {"EXIT_OUT_OF_RANGE", "OUT_OF_RANGE", 255, "exit", "Exit status out of range", "Shells often surface wrapped or invalid exit values as 255.", "Return an explicit value in the 0-255 range.", FAULT_KIND_EXIT}
};

const size_t exit_faults_count = sizeof(exit_faults) / sizeof(exit_faults[0]);

static const char *kind_title(fault_kind_t kind)
{
    switch (kind) {
    case FAULT_KIND_ERRNO:
        return "Linux errno";
    case FAULT_KIND_SIGNAL:
        return "Linux signals";
    case FAULT_KIND_EXIT:
        return "Unix exit codes";
    }

    return "Faults";
}

static void print_related(const Fault *fault, const Fault *faults, size_t count)
{
    size_t i;
    int shown;

    shown = 0;
    printf("Related:\n");

    for (i = 0; i < count && shown < 3; i++) {
        if (&faults[i] == fault) {
            continue;
        }

        if (faults[i].kind != fault->kind) {
            continue;
        }

        if (faults[i].code == fault->code) {
            continue;
        }

        if (faults[i].category == NULL || fault->category == NULL) {
            continue;
        }

        if (faults[i].common_causes == NULL || fault->common_causes == NULL) {
            continue;
        }

        if (faults[i].kind == FAULT_KIND_ERRNO && faults[i].code / 10 == fault->code / 10) {
            printf("- %s (%d): %s\n", faults[i].name, faults[i].code, faults[i].description);
            shown++;
            continue;
        }

        if (faults[i].kind == FAULT_KIND_SIGNAL && shown < 3) {
            if ((fault->code <= 15 && faults[i].code <= 15) ||
                (fault->code >= 17 && faults[i].code >= 17 && faults[i].code <= 31)) {
                printf("- %s (%d): %s\n", faults[i].name, faults[i].code, faults[i].description);
                shown++;
            }
        }
    }

    if (shown == 0) {
        printf("- none in the current static table\n");
    }
}

void print_fault(const Fault *fault)
{
    const Fault *table;
    size_t count;

    if (fault == NULL) {
        return;
    }

    table = NULL;
    count = 0;

    switch (fault->kind) {
    case FAULT_KIND_ERRNO:
        table = errno_faults;
        count = errno_faults_count;
        break;
    case FAULT_KIND_SIGNAL:
        table = signal_faults;
        count = signal_faults_count;
        break;
    case FAULT_KIND_EXIT:
        table = exit_faults;
        count = exit_faults_count;
        break;
    }

    printf("%s (%d)\n", fault->name, fault->code);
    printf("Category: %s\n\n", fault->category);
    printf("Meaning:\n%s\n\n", fault->description);
    printf("Common causes:\n%s\n\n", fault->common_causes);
    printf("Possible fixes:\n%s\n\n", fault->possible_fixes);

    if (table != NULL) {
        print_related(fault, table, count);
    }
}

void print_fault_list(const char *title, const Fault *faults, size_t count)
{
    size_t i;

    printf("%s\n", title);
    for (i = 0; i < count; i++) {
        printf("  %-18s %3d  %s\n", faults[i].name, faults[i].code, faults[i].description);
    }
    printf("\n");
}

void print_list(void)
{
    print_fault_list(kind_title(FAULT_KIND_ERRNO), errno_faults, errno_faults_count);
    print_fault_list(kind_title(FAULT_KIND_SIGNAL), signal_faults, signal_faults_count);
    print_fault_list(kind_title(FAULT_KIND_EXIT), exit_faults, exit_faults_count);
}

void print_usage(const char *progname)
{
    printf("Usage: %s <name|number|--list>\n\n", progname);
    printf("Examples:\n");
    printf("  %s EACCES\n", progname);
    printf("  %s 13\n", progname);
    printf("  %s SIGSEGV\n", progname);
    printf("  %s 139\n", progname);
    printf("  %s --list\n", progname);
}

void print_exit_status_hint(int code)
{
    if (code == 0) {
        printf("Exit status hint:\n0 means success.\n");
        return;
    }

    if (code >= 1 && code <= 125) {
        printf("Exit status hint:\n%d is a non-zero application-defined exit status.\n", code);
        return;
    }

    if (code == 126) {
        printf("Exit status hint:\n126 usually means the command was found but could not be executed.\n");
        return;
    }

    if (code == 127) {
        printf("Exit status hint:\n127 usually means command not found.\n");
        return;
    }

    if (code >= 128) {
        printf("Exit status hint:\n%d may encode a signal as 128 + signal number.\n", code);
        return;
    }

    printf("Exit status hint:\n%d is application-specific.\n", code);
}
