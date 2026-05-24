#include <stdio.h>
#include <string.h>

#include "fault.h"
#include "search.h"
#include "translation.h"

static int parse_language_name(const char *name, fault_language_t *language)
{
    if (strcmp(name, "en") == 0) {
        *language = FAULT_LANG_EN;
        return 1;
    }

    if (strcmp(name, "uk") == 0 || strcmp(name, "ua") == 0) {
        *language = FAULT_LANG_UK;
        return 1;
    }

    return 0;
}

static void print_number_matches(const int code, const fault_language_t language)
{
    const Fault *matches[3];

    const size_t count = collect_faults_by_code(code, matches, 3);

    for (size_t i = 0; i < count; i++) {
        if (i > 0) {
            printf("\n");
        }
        print_fault(matches[i], language);
    }

    const Fault *signal = decode_exit_signal(code);

    if (signal != NULL) {
        if (count > 0) {
            printf("\n");
        }

        if (language == FAULT_LANG_UK) {
            printf("Похідний код завершення від сигналу:\n");
            printf("%d = 128 + %d -> %s (%s)\n",
                   code, signal->code, signal->name, fault_description_for_lang(signal, language));
        } else {
            printf("Derived signal exit:\n");
            printf("%d = 128 + %d -> %s (%s)\n",
                   code, signal->code, signal->name, fault_description_for_lang(signal, language));
        }
    } else if (count == 0) {
        if (language == FAULT_LANG_UK) {
            printf("Для коду %d немає вбудованого запису.\n\n", code);
        } else {
            printf("No built-in fault entry matches code %d.\n\n", code);
        }
        print_exit_status_hint(code, language);
    } else {
        printf("\n");
        print_exit_status_hint(code, language);
    }
}

int main(const int argc, char **argv)
{
    fault_language_t language;

    int code;

    const char *query = NULL;

    language = FAULT_LANG_EN;

    int help_mode = 0;
    int list_mode = 0;

    if (!validate_fault_catalog()) {
        fprintf(stderr, "sysfault: internal fault catalog error: %s\n",
                fault_catalog_validation_error());
        return 2;
    }

    if (!validate_fault_translations()) {
        fprintf(stderr, "sysfault: internal translation error: %s\n",
                fault_translation_validation_error());
        return 2;
    }

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--lang") == 0 || strcmp(argv[i], "-l") == 0) {
            if (i + 1 >= argc) {
                fprintf(stderr, "sysfault: missing language after %s\n", argv[i]);
                return 1;
            }

            if (!parse_language_name(argv[i + 1], &language)) {
                fprintf(stderr, "sysfault: unsupported language: %s\n", argv[i + 1]);
                fprintf(stderr, "Supported languages: en, uk\n");
                return 1;
            }

            i++;
            continue;
        }

        if (strcmp(argv[i], "--list") == 0) {
            list_mode = 1;
            continue;
        }

        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help_mode = 1;
            continue;
        }

        if (query == NULL) {
            query = argv[i];
            continue;
        }

        fprintf(stderr, "sysfault: unexpected argument: %s\n", argv[i]);
        return 1;
    }

    if (help_mode) {
        print_usage(argv[0], language);
        return 0;
    }

    if (list_mode) {
        if (query != NULL) {
            fprintf(stderr, "sysfault: --list does not take a lookup value\n");
            return 1;
        }

        print_list(language);
        return 0;
    }

    if (query == NULL) {
        print_usage(argv[0], language);
        return 1;
    }

    if (parse_code(query, &code)) {
        print_number_matches(code, language);
        return 0;
    }

    const Fault *fault = find_any_by_name(query);
    if (fault == NULL) {
        if (language == FAULT_LANG_UK) {
            fprintf(stderr, "sysfault: невідомий код або назва: %s\n", query);
            fprintf(stderr, "Спробуй --list, щоб побачити відомі errno, сигнали та коди завершення.\n");
        } else {
            fprintf(stderr, "sysfault: unknown code or name: %s\n", query);
            fprintf(stderr, "Try --list to see known errno, signals, and exit codes.\n");
        }
        return 1;
    }

    print_fault(fault, language);
    return 0;
}
