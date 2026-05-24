#!/bin/sh

set -eu

ROOT_DIR=$(CDPATH= cd -- "$(dirname "$0")/.." && pwd)
BIN="$ROOT_DIR/build/sysfault"

check_contains() {
    output=$1
    needle=$2

    printf '%s' "$output" | grep -F "$needle" >/dev/null
}

output=$("$BIN" EACCES)
check_contains "$output" "EACCES (13)"
check_contains "$output" "Category: errno"

output=$("$BIN" 13)
check_contains "$output" "SIGPIPE (13)"
check_contains "$output" "EACCES (13)"

output=$("$BIN" SIGSEGV)
check_contains "$output" "SIGSEGV (11)"

output=$("$BIN" 139)
check_contains "$output" "EXIT_SIGSEGV (139)"
check_contains "$output" "139 = 128 + 11 -> SIGSEGV"

output=$("$BIN" --list)
check_contains "$output" "Linux errno"
check_contains "$output" "Linux signals"
check_contains "$output" "Unix exit codes"

output=$("$BIN" --lang uk EACCES)
check_contains "$output" "Категорія: errno"
check_contains "$output" "Доступ заборонено"

output=$("$BIN" --lang uk 139)
check_contains "$output" "Код завершення сегментаційної помилки"
check_contains "$output" "Похідний код завершення від сигналу"

printf 'test_basic.sh: all checks passed\n'
