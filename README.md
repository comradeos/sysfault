# sysfault

`sysfault` is a small C CLI for searching system faults by code, name, description text, or global catalog id.

Current catalog includes:

- Linux `errno`, signals, and exit codes
- macOS `errno`, signals, and exit codes
- English output by default
- Ukrainian output with `-ua`
- OS-aware filtering with `-linux`, `-macos`, `-windows`

## Build

```sh
make build
```

Binary output:

```text
build/sysfault
```

## Test

```sh
make test
```

## Query model

`sysfault` chooses the current operating system automatically at startup.

- On Linux it searches Linux entries by default
- On macOS it searches macOS entries by default
- On Windows it searches Windows entries by default

You can override the filter explicitly:

```sh
./build/sysfault 13 -linux
./build/sysfault 13 -macos
./build/sysfault 13 -windows
```

## Language flags

```sh
./build/sysfault -en EACCES
./build/sysfault -ua EACCES
```

If no language flag is passed, English is used.

## Usage examples

Search by system code:

```sh
./build/sysfault 13
./build/sysfault 139
./build/sysfault 13 -linux
```

Search by system name or alias:

```sh
./build/sysfault EACCES
./build/sysfault SIGSEGV
./build/sysfault SEGV
```

Search by text inside the localized description:

```sh
./build/sysfault -f segmentation
./build/sysfault -ua -f "сегмента"
```

Search by global catalog id:

```sh
./build/sysfault -id 13
./build/sysfault -ua -id 101
```

List all entries for the selected OS scope:

```sh
./build/sysfault --list
./build/sysfault -linux --list
```

## Output format

Multiple matches are shown as a compact list:

```text
13: linux, error, EACCES (13)
73: linux, signal, SIGPIPE (13)
```

Single matches additionally show detailed text:

```text
13: linux, error, EACCES (13)

OS: linux
Type: error
System code: 13
System name: EACCES

Description:
Permission denied
```

## Project structure

```text
sysfault/
├── Makefile
├── README.md
├── src/
│   ├── main.c
│   ├── fault.h
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
