#include "statistics.h"

stat_t stats = {0};

float findPercent(int part, int all) {
    return all ? (part * 100.0 / all) : 0;
}

void printStat(stat_t *stats) {
    stats->ntotal = stats->nreg + 
                    stats->nchr + 
                    stats->nblk + 
                    stats->nfifo + 
                    stats->nslink + 
                    stats->nsock + 
                    stats->ndir;

    printf(YELLOW
        "\n\n\n File stats \
         \n ------------------------------------------------ \
         \n regular (-):                    %5ld (%4.2f %%) \
         \n directory (d):                  %5ld (%4.2f %%) \
         \n block device (b):               %5ld (%4.2f %%) \
         \n character device (c):           %5ld (%4.2f %%) \
         \n named pipe (fifo) (p):          %5ld (%4.2f %%) \
         \n symbolic link (l):              %5ld (%4.2f %%) \
         \n socket (s):                     %5ld (%4.2f %%) \
         \n ------------------------------------------------ \
         \n Total:                          %5ld \
         \n ------------------------------------------------\n\n" RESET, 
         stats->nreg, findPercent(stats->nreg, stats->ntotal),
         stats->ndir, findPercent(stats->ndir, stats->ntotal),
         stats->nblk, findPercent(stats->nblk, stats->ntotal),
         stats->nchr, findPercent(stats->nchr, stats->ntotal),
         stats->nfifo, findPercent(stats->nfifo, stats->ntotal),
         stats->nslink, findPercent(stats->nslink, stats->ntotal),
         stats->nsock, findPercent(stats->nsock, stats->ntotal),
         stats->ntotal
    );
}

void incStat(struct stat *mode, stat_t *stats) {
    switch (mode->st_mode & S_IFMT) {
        case S_IFREG:
            stats->nreg++;
            break;
        case S_IFBLK:
            stats->nblk++;
            break;
        case S_IFCHR:
            stats->nchr++;
            break;
        case S_IFIFO:
            stats->nfifo++;
            break;
        case S_IFLNK:
            stats->nslink++;
            break;
        case S_IFSOCK:
            stats->nsock++;
            break;
        case S_IFDIR:
            stats->ndir++;
            break;
    }
}