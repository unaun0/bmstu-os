#ifndef STATISTICS
#define STATISTICS

#include <sys/stat.h>
#include <stdio.h>

#define RED     "\x1b[38;5;196m"
#define GREEN   "\x1b[38;5;46m"
#define YELLOW  "\x1b[38;5;226m"
#define BLUE    "\x1b[38;5;33m"
#define MAGENTA "\x1b[38;5;201m"
#define CYAN    "\x1b[38;5;51m"
#define RESET   "\x1b[0m"

#define TEST    "\x1b[37m"

typedef struct stat_s {
    long nreg;
    long ndir;
    long nblk;
    long nchr;
    long nfifo;
    long nslink;
    long nsock;
    long ntotal;
} stat_t;

extern stat_t stats;

void printStat(stat_t *stats);
void incStat(struct stat *mode, stat_t *stats);

#endif
