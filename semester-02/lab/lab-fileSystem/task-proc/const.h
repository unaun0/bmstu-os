#ifndef CONST_HEADER
#define CONST_HEADER
const char *WITH_DESCR[] = {
        "(1) pid  '%s' The process ID.\n",
        "(2) comm '%s' (The filename of the executable, in parentheses).\n",
        "(3) state  '%s' (one of the characters indicating process state).\n",
        "(4) ppid  '%s' (The PID of the parent of this process).\n",
        "(5) pgrp  '%s' (The process group ID of the process).\n",
        "(6) session  '%s' (The session ID of the process).\n",
        "(7) tty_nr  '%s' (The controlling terminal of the process).\n",
        "(8) tpgid  '%s' (The ID of the foreground process group of the controlling terminal of the process).\n",
        "(9) flags  '%s' (The kernel flags word of the process).\n",
        "(10) minflt  '%s' (The number of minor faults the process has made which have not required loading a memory page from disk).\n",
        "(11) cminflt  '%s' (The number of minor faults that the process's waited-for children have made).\n",
        "(12) majflt  '%s' (The number of major faults the process has made which have required  loading  a  memory  page from disk).\n",
        "(13) cmajflt  '%s' (The number of major faults that the process's waited-for children have made).\n",
        "(14) utime  '%s' (Amount  of  time  that  this process has been scheduled in user mode, measured in clock ticks).\n",
        "(15) stime  '%s' (Amount of time that this process has been scheduled in kernel mode, measured in  clock  ticks).\n",
        "(16) cutime  '%s' (Amount of time that this process's waited-for children have been scheduled in user mode, measured in clock ticks).\n",
        "(17) cstime  '%s' (Amount  of  time  that this process's waited-for children have been scheduled in kernel mode, measured in clock ticks).\n",
        "(18) priority  '%s' (For processes running a real-time scheduling policy).\n",
        "(19) nice  '%s' (The nice value (see setpriority(2)), a value in the range 19 (low priority) to -20 (high priority)).\n",
        "(20) num_threads  '%s' (Number  of threads in this process (since Linux 2).6).  Before kernel 2.6, this field was hardcoded to 0 as a placeholder for an earlier removed field.\n",
        "(21) itrealvalue  '%s' (The time in jiffies before the next SIGALRM is sent to the process due to an interval  timer).\n",
        "(22) starttime  '%s' (The  time the process started after system boot).\n",
        "(23) vsize  '%s' (Virtual memory size in bytes).\n",
        "(24) rss  '%s' (Resident  Set  Size:  number of pages the process has in real memory).\n",
        "(25) rsslim  '%s' (Current soft limit in bytes on the rss of the process).\n",
        "(26) startcode  '%s' (The address above which program text can run).\n",
        "(27) endcode  '%s'   (The address below which program text can run).\n",
        "(28) startstack  '%s'  T(he address of the start (i).e., bottom) of the stack.\n",
        "(29) kstkesp  '%s'  T(he current value of ESP (stack pointer), as found in the kernel stack page for the process).\n",
        "(30) kstkeip  '%s'  T(he current EIP (instruction pointer)).\n",
        "(31) signal  '%s' (The  bitmap of pending signals, displayed as a decimal number).\n",
        "(32) blocked  '%s' (The bitmap of blocked signals, displayed as a decimal number).\n",
        "(33) sigignore  '%s' (The  bitmap of ignored signals, displayed as a decimal number).\n",
        "(34) sigcatch  '%s' (The bitmap of caught signals, displayed as a decimal number).\n",
        "(35) wchan  '%s'  T(his  is  the  \"channel\" in which the process is waiting).\n",
        "(36) nswap  '%s' (Number of pages swapped (not maintained)).\n",
        "(37) cnswap  '%s' (Cumulative nswap for child processes (not maintained)).\n",
        "(38) exit_signal  '%s'  (since Linux 2.1.22) (Signal to be sent to parent when we die).\n",
        "(39) processor  '%s'  (since Linux 2.2.8) (CPU number last executed on).\n",
        "(40) rt_priority  '%s'  (since Linux 2.5.19) (Real-time  scheduling priority, a number in the range 1 to 99 for processes scheduled under a real-time policy, or 0, for non-real-time processes).\n",
        "(41) policy  '%s'  (since Linux 2.5.19) (Scheduling policy).\n",
        "(42) delayacct_blkio_ticks  '%s'  (since Linux 2.6.18) (Aggregated block I/O delays, measured in clock ticks (centiseconds)).\n",
        "(43) guest_time  '%s'  (since Linux 2.6.24) (Guest  time  of  the process (time spent running a virtual CPU for a guest operating system), measured in clock ticks).\n",
        "(44) cguest_time  '%s'  (since Linux 2.6.24) (Guest  time   of   the   process's   children,   measured   in   clock   ticks).\n",
        "(45) start_data  '%s'  (since Linux 3.3)  A(ddress above which program initialized and uninitialized (BSS) data are placed).\n",
        "(46) end_data  '%s'  (since Linux 3.3)  A(ddress below which program initialized and uninitialized (BSS) data are placed).\n",
        "(47) start_brk  '%s'  (since Linux 3.3)  A(ddress above which program heap can be expanded with brk(2)).\n",
        "(48) arg_start  '%s'  (since Linux 3.5)  A(ddress above which program command-line arguments (argv) are placed).\n",
        "(49) arg_end  '%s'  (since Linux 3.5)  A(ddress below program command-line arguments (argv) are placed).\n",
        "(50) env_start  '%s'  (since Linux 3.5)  A(ddress above which program environment is placed).\n",
        "(51) env_end  '%s'  (since Linux 3.5)  A(ddress below which program environment is placed).\n",
        "(52) exit_code  '%s'  (since Linux 3.5)  T(he thread's exit status in the form reported by waitpid(2)).\n"
};

const char *NO_DESCR[] = {
        "(1) pid  '%s'\n",
        "(2) comm '%s'\n",
        "(3) state  '%s'\n",
        "(4) ppid  '%s'\n",
        "(5) pgrp  '%s'\n",
        "(6) session  '%s'\n",
        "(7) tty_nr  '%s'\n",
        "(8) tpgid  '%s'\n",
        "(9) flags  '%s'\n",
        "(10) minflt  '%s'\n",
        "(11) cminflt  '%s'\n",
        "(12) majflt  '%s'\n",
        "(13) cmajflt  '%s'\n",
        "(14) utime  '%s'\n",
        "(15) stime  '%s'\n",
        "(16) cutime  '%s'\n",
        "(17) cstime  '%s'\n",
        "(18) priority  '%s'\n",
        "(19) nice  '%s'\n",
        "(20) num_threads  '%s'\n",
        "(21) itrealvalue  '%s'\n",
        "(22) starttime  '%s'\n",
        "(23) vsize  '%s'\n",
        "(24) rss  '%s'\n",
        "(25) rsslim  '%s'\n",
        "(26) startcode  '%s'\n",
        "(27) endcode  '%s'\n",
        "(28) startstack  '%s'\n",
        "(29) kstkesp  '%s'\n",
        "(30) kstkeip  '%s'\n",
        "(31) signal  '%s'\n",
        "(32) blocked  '%s'\n",
        "(33) sigignore  '%s'\n",
        "(34) sigcatch  '%s'\n",
        "(35) wchan  '%s'\n",
        "(36) nswap  '%s'\n",
        "(37) cnswap  '%s'\n",
        "(38) exit_signal  '%s'\n",
        "(39) processor  '%s'\n",
        "(40) rt_priority  '%s'\n",
        "(41) policy  '%s'\n",
        "(42) delayacct_blkio_ticks  '%s'\n",
        "(43) guest_time  '%s'\n",
        "(44) cguest_time  '%s'\n",
        "(45) start_data  '%s'\n",
        "(46) end_data  '%s'\n",
        "(47) start_brk  '%s'\n",
        "(48) arg_start  '%s'\n",
        "(49) arg_end  '%s'\n",
        "(50) env_start  '%s'\n",
        "(51) env_end  '%s'\n",
        "(52) exit_code  '%s'\n"
};

const char *STATM_PATTERNS[] = {
        "size     %s  (1)\n",
        "resident %s  (2)\n",
        "shared   %s  (3)\n",
        "text     %s  (4)\n",
        "lib      %s  (5)\n",
        "data     %s  (6)\n",
        "dt       %s  (7)\n"
};
#endif
