# sysfault

`sysfault` is a small C CLI that explains Linux and Unix failures in a human-readable format.

Current MVP covers:

- Linux/POSIX `errno`
- Linux signals
- Common Unix exit codes
- `128 + signal` crash exits like `139 -> SIGSEGV`
- Short cause and fix hints
- Built-in static tables with `--list`

## Project structure

```text
sysfault/
├── Makefile
├── README.md
├── src/
│   ├── main.c
│   ├── fault.h
│   ├── fault.c
│   ├── errno_data.c
│   ├── signal_data.c
│   ├── search.c
│   └── search.h
└── tests/
    └── test_basic.sh
```

## Build

```sh
make
```

This produces `./build/sysfault`.

## Test

```sh
make test
```

## Usage

```sh
./build/sysfault EACCES
./build/sysfault 13
./build/sysfault SIGSEGV
./build/sysfault 139
./build/sysfault --list
```

## Example output

```text
EACCES (13)
Category: errno

Meaning:
Permission denied

Common causes:
File permissions, directory permissions, SELinux/AppArmor restrictions.

Possible fixes:
Check file mode, directory traversal permissions, and the current user.
```

## Notes

- Numeric lookup is broad by design. `13` can match both `EACCES` and `SIGPIPE`.
- Exit code lookup also explains derived signal exits such as `139 = 128 + 11`.
- The project intentionally uses static tables and no external dependencies.
