# sysfault

`sysfault` is a small C CLI that explains Linux and Unix failures in a human-readable format.

Current MVP covers:

- Linux/POSIX `errno`
- Linux signals
- Common Unix exit codes
- `128 + signal` crash exits like `139 -> SIGSEGV`
- Short cause and fix hints
- English by default with optional Ukrainian output via `--lang uk`
- Built-in static tables with `--list`
- One shared fault catalog with separate `en` and `uk` translation files

## Project structure

```text
sysfault/
├── Makefile
├── README.md
├── src/
│   ├── main.c
│   ├── fault.h
│   ├── fault_ids.h
│   ├── fault.c
│   ├── fault_catalog.c
│   ├── search.c
│   ├── search.h
│   ├── translation.c
│   ├── translation.h
│   ├── translation_en.c
│   └── translation_uk.c
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
./build/sysfault --lang uk EACCES
./build/sysfault --lang uk --list
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

```text
EACCES (13)
Категорія: errno

Значення:
Доступ заборонено

Типові причини:
Права на файл, права проходу каталогами або обмеження SELinux/AppArmor.

Можливі дії:
Перевір права файла, права проходу каталогами та поточного користувача.
```

## Notes

- Numeric lookup is broad by design. `13` can match both `EACCES` and `SIGPIPE`.
- Exit code lookup also explains derived signal exits such as `139 = 128 + 11`.
- The project intentionally uses static tables and no external dependencies.
- Core metadata lives in `src/fault_catalog.c`, while text is stored separately in `src/translation_en.c` and `src/translation_uk.c`.
