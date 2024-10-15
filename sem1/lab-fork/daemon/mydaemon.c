#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/file.h>
#include <time.h>
#include <pthread.h>

sigset_t mask;

#define LOCKFILE "/var/run/daemon.pid"
#define LOCKMODE (S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH)

void daemonize(const char *cmd)
{
    int i, fd0, fd1, fd2;
    pid_t pid;
    struct rlimit rl;
    umask(0);
    if (getrlimit(RLIMIT_NOFILE, &rl) < 0) {
        fprintf(stderr, "%s: unable to get the maximum file descriptor number", cmd);
        exit(1);
    }
    if ((pid = fork()) == -1) {
        perror("can't fork");
        exit(1) ;
    }
    if (pid != 0)
        exit(0);
    if (setsid() == -1) {
        perror("can't setsid");
        exit(1) ;
    }
    if (signal(SIGHUP, SIG_IGN) == SIG_ERR) {
        perror("can't irnore SIGHUP");
        exit(1);
    }
    if (chdir("/") == -1) {
        perror("can't chdir");
        exit(1);
    }
    if (rl.rlim_max == RLIM_INFINITY)
        rl.rlim_max = 1024;
    for (i = 0; i < rl.rlim_max; i++)
        close(i);
    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);
    openlog(cmd, LOG_CONS, LOG_DAEMON);
    if (fd0 != 0 || fd1 != 1 || fd2 != 2) {
        syslog(LOG_ERR, "error stdin, sidout, stderr: %d %d %d", fd0, fd1, fd2);
        exit(1);
    }
}

int lockfile(int fd)
{
    struct flock fl;

    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    return fcntl(fd, F_SETLK, &fl);
}

int already_running(void)
{
    int fd;
    char buf[16];

    fd = open(LOCKFILE, O_WRONLY|O_CREAT, LOCKMODE);
    if (fd == -1) {
        syslog(LOG_ERR, "unable to open %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    if (lockfile(fd) == -1) {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }
        syslog(LOG_ERR, "unable to block %s: %s", LOCKFILE, strerror(errno));
        exit(1);
    }
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return(0);
}

void *thr_fn(void *arg)
{
    int err, signo;

    for (;;) {
        err = sigwait(&mask, &signo);
        if (err != 0) {
            syslog(LOG_ERR, "error calling sigwait function\n");
            exit(1);
        }
        switch (signo) {
        case SIGHUP:
            syslog(LOG_INFO, "SIGHUP received; reading configuration file\n");

            break;
        case SIGTERM:
            syslog(LOG_INFO, "SIGTERM received; exiting\n");
            exit(0);
        default:
            syslog(LOG_INFO, "unexpected signal %d received\n", signo);
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    time_t t;
    int fd, err;
    pthread_t tid;
    char *cmd;
    struct sigaction sa;

    printf("login before daemonize:%s\n", getlogin());
    
    if ((cmd = strrchr(argv[0], '/')) == NULL)
        cmd = argv[0];
    else
        cmd++;
        
    daemonize(cmd);
    if (already_running()) {
        syslog(LOG_ERR, "%s: daemon already running", cmd);
        exit(1);
    }
    sa.sa_handler = SIG_DFL;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGHUP, &sa, NULL) == -1) {
        syslog(LOG_ERR, "unable to restore SIG_DFL action for SIGHUP");
        exit(1);
    }
    sigfillset(&mask);
    if ((err = pthread_sigmask(SIG_BLOCK, &mask, NULL)) != 0) {
        syslog(LOG_ERR, "error executing SIG_BLOCK operation");
        exit(1);
    }
    err = pthread_create(&tid, NULL, thr_fn, 0);
    if (err != 0) {
        syslog(LOG_ERR, "unable to create thread");
        exit(1);
    }
    syslog(LOG_INFO, "Login after daemonize:%s\n", getlogin());
    while (1) {
        t = time(NULL);
        struct tm tm = *localtime(&t);
        syslog(LOG_INFO, "%s: Current time is: %02d:%02d:%02d\n",
                cmd, tm.tm_hour, tm.tm_min, tm.tm_sec);
        sleep(1);
    }
    return 0;
}