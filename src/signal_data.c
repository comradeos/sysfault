#include "fault.h"

const Fault signal_faults[] = {
    {"SIGHUP", "HUP", 1, "signal", "Hangup", "The controlling terminal disappeared or a supervisor asked the process to reload or stop.", "Check the parent process, terminal state, and daemon reload behavior.", FAULT_KIND_SIGNAL},
    {"SIGINT", "INT", 2, "signal", "Interrupt", "The process received an interactive interrupt, usually Ctrl-C.", "Handle cleanup on interruption or retry the command without interruption.", FAULT_KIND_SIGNAL},
    {"SIGQUIT", "QUIT", 3, "signal", "Quit", "The process received a quit request, often with a core dump.", "Inspect the core dump and determine why the process was force-quit.", FAULT_KIND_SIGNAL},
    {"SIGILL", "ILL", 4, "signal", "Illegal instruction", "The CPU tried to execute an invalid or unsupported instruction.", "Verify binary architecture, compiler flags, and memory corruption.", FAULT_KIND_SIGNAL},
    {"SIGTRAP", "TRAP", 5, "signal", "Trace or breakpoint trap", "A debugger breakpoint or tracing event stopped execution.", "Check whether a debugger, sanitizer, or trap instruction is active.", FAULT_KIND_SIGNAL},
    {"SIGABRT", "ABRT", 6, "signal", "Abort signal", "The program called abort() or a fatal runtime assertion fired.", "Read assertions, stderr output, and recent fatal checks.", FAULT_KIND_SIGNAL},
    {"SIGBUS", "BUS", 7, "signal", "Bus error", "The process touched memory with invalid alignment or impossible backing.", "Check mmap usage, alignment, and underlying file truncation.", FAULT_KIND_SIGNAL},
    {"SIGFPE", "FPE", 8, "signal", "Floating point exception", "Arithmetic trapped on divide-by-zero or another invalid numeric operation.", "Review arithmetic inputs and guard exceptional numeric cases.", FAULT_KIND_SIGNAL},
    {"SIGKILL", "KILL", 9, "signal", "Killed", "The process was terminated immediately by the kernel or another process.", "Check OOM killer messages, supervisors, and external kill commands.", FAULT_KIND_SIGNAL},
    {"SIGUSR1", "USR1", 10, "signal", "User-defined signal 1", "The application uses this signal for custom control flow.", "Inspect application documentation or signal handlers.", FAULT_KIND_SIGNAL},
    {"SIGSEGV", "SEGV", 11, "signal", "Segmentation fault", "Invalid memory access, null pointer dereference, or use-after-free.", "Check pointers, array bounds, memory ownership, and recent unsafe writes.", FAULT_KIND_SIGNAL},
    {"SIGUSR2", "USR2", 12, "signal", "User-defined signal 2", "The application uses this second signal for custom behavior.", "Inspect how the program handles SIGUSR2.", FAULT_KIND_SIGNAL},
    {"SIGPIPE", "PIPE", 13, "signal", "Broken pipe", "The process wrote after the pipe or socket reader had already closed.", "Handle peer disconnects and consider ignoring SIGPIPE if appropriate.", FAULT_KIND_SIGNAL},
    {"SIGALRM", "ALRM", 14, "signal", "Alarm clock", "A timer created by alarm() or setitimer() expired.", "Review timeouts and internal timer setup.", FAULT_KIND_SIGNAL},
    {"SIGTERM", "TERM", 15, "signal", "Termination signal", "A polite shutdown request asked the process to stop cleanly.", "Check service managers, scripts, and shutdown hooks.", FAULT_KIND_SIGNAL},
    {"SIGCHLD", "CHLD", 17, "signal", "Child exited", "A child process changed state and may need to be reaped.", "Call wait or waitpid and verify child lifecycle handling.", FAULT_KIND_SIGNAL},
    {"SIGCONT", "CONT", 18, "signal", "Continued", "A previously stopped process was resumed.", "Check job control or external process management.", FAULT_KIND_SIGNAL},
    {"SIGSTOP", "STOP", 19, "signal", "Stopped", "The process was paused immediately and cannot catch or ignore this signal.", "Inspect job control, ptrace, or external pause requests.", FAULT_KIND_SIGNAL},
    {"SIGTSTP", "TSTP", 20, "signal", "Terminal stop", "Interactive job control asked the process to stop, usually with Ctrl-Z.", "Foreground the job again or disable terminal stop behavior if needed.", FAULT_KIND_SIGNAL},
    {"SIGTTIN", "TTIN", 21, "signal", "Background read from tty", "A background process tried to read from its controlling terminal.", "Move the job to foreground or redirect terminal input.", FAULT_KIND_SIGNAL},
    {"SIGTTOU", "TTOU", 22, "signal", "Background write to tty", "A background process tried to write to its controlling terminal.", "Move the job to foreground or redirect terminal output.", FAULT_KIND_SIGNAL},
    {"SIGURG", "URG", 23, "signal", "Urgent I/O condition", "Out-of-band or urgent socket data became available.", "Inspect socket handlers and urgent-data behavior.", FAULT_KIND_SIGNAL},
    {"SIGXCPU", "XCPU", 24, "signal", "CPU time limit exceeded", "The process used more CPU time than allowed by resource limits.", "Raise limits or optimize the workload.", FAULT_KIND_SIGNAL},
    {"SIGXFSZ", "XFSZ", 25, "signal", "File size limit exceeded", "A write would grow a file beyond its allowed limit.", "Raise file size limits or write somewhere else.", FAULT_KIND_SIGNAL},
    {"SIGVTALRM", "VTALRM", 26, "signal", "Virtual timer expired", "A per-process virtual timer expired while the program was executing.", "Review profiling or timeout-related timer setup.", FAULT_KIND_SIGNAL},
    {"SIGPROF", "PROF", 27, "signal", "Profiling timer expired", "A profiling timer fired, often for sampling or instrumentation.", "Inspect profilers or application timers.", FAULT_KIND_SIGNAL},
    {"SIGWINCH", "WINCH", 28, "signal", "Window size changed", "The terminal window dimensions changed.", "Refresh terminal UI layout after resize.", FAULT_KIND_SIGNAL},
    {"SIGIO", "IO", 29, "signal", "I/O now possible", "An asynchronous descriptor became ready for I/O.", "Check async I/O handlers and readiness logic.", FAULT_KIND_SIGNAL},
    {"SIGPWR", "PWR", 30, "signal", "Power failure", "The system reported a power-related event.", "Inspect system logs and power handling hooks.", FAULT_KIND_SIGNAL},
    {"SIGSYS", "SYS", 31, "signal", "Bad system call", "The process made an invalid syscall or a seccomp rule blocked it.", "Check syscall numbers, seccomp filters, and target platform support.", FAULT_KIND_SIGNAL}
};

const size_t signal_faults_count = sizeof(signal_faults) / sizeof(signal_faults[0]);
