#include <stdio.h>
#include <string.h>

#include "fault.h"
#include "search.h"
#include "translation.h"

typedef enum {
    QUERY_MODE_NONE,
    QUERY_MODE_LIST,
    QUERY_MODE_ID,
    QUERY_MODE_TEXT,
    QUERY_MODE_VALUE
} query_mode_t;

static int parse_os_flag(const char *arg, os_t *selected_os)
{
    if (strcmp(arg, "-linux") == 0) {
        *selected_os = LINUX;
        return 1;
    }

    if (strcmp(arg, "-macos") == 0) {
        *selected_os = MACOS;
        return 1;
    }

    if (strcmp(arg, "-windows") == 0) {
        *selected_os = WINDOWS;
        return 1;
    }

    return 0;
}

static int parse_language_flag(const char *arg, language_t *language)
{
    if (strcmp(arg, "-en") == 0) {
        *language = EN;
        return 1;
    }

    if (strcmp(arg, "-ua") == 0) {
        *language = UK;
        return 1;
    }

    return 0;
}

static void print_no_matches_for_query(const query_mode_t mode, const char *query, const language_t language)
{
    if (language == UK) {
        switch (mode) {
        case QUERY_MODE_ID:
            fprintf(stderr, "sysfault: запис з id %s не знайдено\n", query);
            return;
        case QUERY_MODE_TEXT:
            fprintf(stderr, "sysfault: за текстом \"%s\" нічого не знайдено\n", query);
            return;
        case QUERY_MODE_VALUE:
            fprintf(stderr, "sysfault: для запиту \"%s\" нічого не знайдено\n", query);
            return;
        case QUERY_MODE_LIST:
        case QUERY_MODE_NONE:
            break;
        }
    }

    switch (mode) {
    case QUERY_MODE_ID:
        fprintf(stderr, "sysfault: no entry found for id %s\n", query);
        return;
    case QUERY_MODE_TEXT:
        fprintf(stderr, "sysfault: no matches found for text \"%s\"\n", query);
        return;
    case QUERY_MODE_VALUE:
        fprintf(stderr, "sysfault: no matches found for query \"%s\"\n", query);
        return;
    case QUERY_MODE_LIST:
    case QUERY_MODE_NONE:
        break;
    }
}

int main(const int argc, char **argv)
{
    const Fault *matches[FAULT_CATALOG_SIZE];
    language_t language = EN;
    const os_t detected_os = detect_current_os();
    os_t selected_os = detected_os;
    query_mode_t mode = QUERY_MODE_NONE;
    const char *query = NULL;
    size_t display_id = 0;
    int code = 0;
    int help_mode = 0;

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
        os_t parsed_os;

        if (parse_language_flag(argv[i], &language)) {
            continue;
        }

        if (parse_os_flag(argv[i], &parsed_os)) {
            selected_os = parsed_os;
            continue;
        }

        if (strcmp(argv[i], "--help") == 0 || strcmp(argv[i], "-h") == 0) {
            help_mode = 1;
            continue;
        }

        if (strcmp(argv[i], "--list") == 0) {
            if (mode != QUERY_MODE_NONE) {
                fprintf(stderr, "sysfault: only one query mode can be used at a time\n");
                return 1;
            }

            mode = QUERY_MODE_LIST;
            continue;
        }

        if (strcmp(argv[i], "-id") == 0) {
            if (mode != QUERY_MODE_NONE) {
                fprintf(stderr, "sysfault: only one query mode can be used at a time\n");
                return 1;
            }

            if (i + 1 >= argc) {
                fprintf(stderr, "sysfault: missing id after -id\n");
                return 1;
            }

            if (!parse_fault_id(argv[i + 1], &display_id)) {
                fprintf(stderr, "sysfault: invalid id: %s\n", argv[i + 1]);
                return 1;
            }

            query = argv[i + 1];
            mode = QUERY_MODE_ID;
            i++;
            continue;
        }

        if (strcmp(argv[i], "-f") == 0) {
            if (mode != QUERY_MODE_NONE) {
                fprintf(stderr, "sysfault: only one query mode can be used at a time\n");
                return 1;
            }

            if (i + 1 >= argc) {
                fprintf(stderr, "sysfault: missing text after -f\n");
                return 1;
            }

            query = argv[i + 1];
            mode = QUERY_MODE_TEXT;
            i++;
            continue;
        }

        if (argv[i][0] == '-') {
            fprintf(stderr, "sysfault: unknown option: %s\n", argv[i]);
            return 1;
        }

        if (mode != QUERY_MODE_NONE || query != NULL) {
            fprintf(stderr, "sysfault: unexpected argument: %s\n", argv[i]);
            return 1;
        }

        query = argv[i];
        mode = QUERY_MODE_VALUE;
    }

    if (help_mode) {
        print_usage(argv[0], detected_os);
        return 0;
    }

    if (mode == QUERY_MODE_NONE) {
        print_usage(argv[0], detected_os);
        return 1;
    }

    if (mode == QUERY_MODE_LIST) {
        if (query != NULL) {
            fprintf(stderr, "sysfault: --list does not take another query\n");
            return 1;
        }

        print_list(selected_os, language);
        return 0;
    }

    size_t count = 0;

    switch (mode) {
    case QUERY_MODE_ID:
        count = collect_faults_by_id(display_id, selected_os, matches, FAULT_CATALOG_SIZE);
        break;
    case QUERY_MODE_TEXT:
        count = collect_faults_by_description(query, selected_os, language, matches, FAULT_CATALOG_SIZE);
        break;
    case QUERY_MODE_VALUE:
        if (parse_code(query, &code)) {
            count = collect_faults_by_code(code, selected_os, matches, FAULT_CATALOG_SIZE);
        } else {
            count = collect_faults_by_name(query, selected_os, matches, FAULT_CATALOG_SIZE);
        }
        break;
    case QUERY_MODE_LIST:
    case QUERY_MODE_NONE:
        break;
    }

    if (count == 0) {
        print_no_matches_for_query(mode, query, language);
        return 1;
    }

    print_fault_results(matches, count, language);
    return 0;
}
