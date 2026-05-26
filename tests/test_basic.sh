#!/bin/sh

set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
BIN="$ROOT_DIR/build/sysfault"

check_contains() {
    output=$1
    needle=$2

    printf '%s' "$output" | grep -F "$needle" >/dev/null
}

OS_NAME=linux
OFFSET=0

if [ "$(uname -s)" = "Darwin" ]; then
    OS_NAME=macos
    OFFSET=103
fi

EACCES_ID=$((13 + OFFSET))
SIGPIPE_ID=$((73 + OFFSET))
SIGSEGV_ID=$((71 + OFFSET))
EXIT_SIGSEGV_ID=$((101 + OFFSET))
LIST_FIRST_ID=$((1 + OFFSET))
LIST_EXIT_SUCCESS_ID=$((91 + OFFSET))

output=$("$BIN" 13)
check_contains "$output" "${EACCES_ID}: ${OS_NAME}, error, EACCES (13)"
check_contains "$output" "${SIGPIPE_ID}: ${OS_NAME}, signal, SIGPIPE (13)"

output=$("$BIN" EACCES)
check_contains "$output" "${EACCES_ID}: ${OS_NAME}, error, EACCES (13)"
check_contains "$output" "Description:"
check_contains "$output" "Permission denied"

output=$("$BIN" -f segmentation)
check_contains "$output" "${SIGSEGV_ID}: ${OS_NAME}, signal, SIGSEGV (11)"
check_contains "$output" "${EXIT_SIGSEGV_ID}: ${OS_NAME}, exit, EXIT_SIGSEGV (139)"

output=$("$BIN" -id "$EACCES_ID")
check_contains "$output" "${EACCES_ID}: ${OS_NAME}, error, EACCES (13)"
check_contains "$output" "System code: 13"

output=$("$BIN" -linux 13)
check_contains "$output" "13: linux, error, EACCES (13)"
check_contains "$output" "73: linux, signal, SIGPIPE (13)"

output=$("$BIN" --list)
check_contains "$output" "${LIST_FIRST_ID}: ${OS_NAME}, error, EPERM (1)"
check_contains "$output" "${LIST_EXIT_SUCCESS_ID}: ${OS_NAME}, exit, EXIT_SUCCESS (0)"

output=$("$BIN" -ua EACCES)
check_contains "$output" "${EACCES_ID}: ${OS_NAME}, помилка, EACCES (13)"
check_contains "$output" "Доступ заборонено"

output=$("$BIN" -ua -id "$EXIT_SIGSEGV_ID")
check_contains "$output" "${EXIT_SIGSEGV_ID}: ${OS_NAME}, вихід, EXIT_SIGSEGV (139)"
check_contains "$output" "Код завершення сегментаційної помилки"

printf 'test_basic.sh: all checks passed\n'
