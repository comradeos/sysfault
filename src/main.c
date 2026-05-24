#include <stdio.h>

#include "fault.h"
#include "search.h"

static void print_number_matches(int code)
{
    const Fault *matches[3];
    const Fault *signal;
    size_t count;
    size_t i;

    count = collect_faults_by_code(code, matches, 3);

    for (i = 0; i < count; i++) {
        if (i > 0) {
            printf("\n");
        }
        print_fault(matches[i]);
    }

    signal = decode_exit_signal(code);
    if (signal != NULL) {
        if (count > 0) {
            printf("\n");
        }
        printf("Derived signal exit:\n%d = 128 + %d -> %s (%s)\n",
               code, signal->code, signal->name, signal->description);
    } else if (count == 0) {
        printf("No built-in fault entry matches code %d.\n\n", code);
        print_exit_status_hint(code);
    } else {
        printf("\n");
        print_exit_status_hint(code);
    }
}

int main(int argc, char **argv)
{
    const Fault *fault;
    int code;

    if (argc != 2) {
        print_usage(argv[0]);
        return 1;
    }

    if (argv[1][0] == '-' && argv[1][1] == '-') {
        if (argv[1][2] == 'l' && argv[1][3] == 'i' && argv[1][4] == 's' && argv[1][5] == 't' && argv[1][6] == '\0') {
            print_list();
            return 0;
        }
    }

    if ((argv[1][0] == '-' && argv[1][1] == 'h' && argv[1][2] == '\0') ||
        (argv[1][0] == '-' && argv[1][1] == '-' && argv[1][2] == 'h' && argv[1][3] == 'e' &&
         argv[1][4] == 'l' && argv[1][5] == 'p' && argv[1][6] == '\0')) {
        print_usage(argv[0]);
        return 0;
    }

    if (parse_code(argv[1], &code)) {
        print_number_matches(code);
        return 0;
    }

    fault = find_any_by_name(argv[1]);
    if (fault == NULL) {
        fprintf(stderr, "sysfault: unknown code or name: %s\n", argv[1]);
        fprintf(stderr, "Try --list to see known errno, signals, and exit codes.\n");
        return 1;
    }

    print_fault(fault);
    return 0;
}
