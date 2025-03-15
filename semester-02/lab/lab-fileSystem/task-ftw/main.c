#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <limits.h>
#include <errno.h>

#include "lib/stack.h"
#include "lib/statistics.h"

#include <time.h>

#define FTW_FILE 1
#define FTW_DIR  2

node_t *stk = NULL;

typedef int fn(const char *, int, int);

int doPath(fn *func, char *pathName, int depth) {
    if (depth < 0){ 
        chdir(pathName);
        return EXIT_SUCCESS;
    }
    struct stat statBuf;
    struct dirent *dirp;
    DIR *dp;
    if (lstat(pathName, &statBuf) == -1) {
        perror("lstat perror: failed to get file stat\n");
        return EXIT_FAILURE;
    }
    if (!S_ISDIR(statBuf.st_mode)) {
        incStat(&statBuf, &stats);
        func(pathName, FTW_FILE, depth);
        return EXIT_SUCCESS;
    }
    func(pathName, FTW_DIR, depth);
    stats.ndir++;
    if ((dp = opendir(pathName)) == NULL) {
        perror("opendir error: directory does not exist or access is not available\n");
        return EXIT_FAILURE;
    }
    if (chdir(pathName) == -1) {
        closedir(dp);
        perror("chdir error: failed to go to directory\n");
        return EXIT_FAILURE;
    }
    depth++;
    node_t *elem = create_node("..", -1);
    push(&stk, elem);
    while ((dirp = readdir(dp)) != NULL) {
        if ((strcmp(dirp->d_name, ".") != 0) && (strcmp(dirp->d_name, "..") != 0)) {
            elem = create_node(dirp->d_name, depth);
            push(&stk, elem);
        }
    }
    if (errno != 0) {
        perror("error readdir");
    }
    if (closedir(dp) == -1) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

static int ftw(char *shortName, fn *func) {
    if (chdir(shortName) == -1) {
        printf("error: directory does not exist or access is not available\n");
        exit(EXIT_FAILURE);
    }
    char cwd[PATH_MAX];
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        printf("error: can't get full path to work dir\n");
        exit(EXIT_FAILURE);
    }
    node_t *elem = create_node(cwd, 0);
    push(&stk, elem); 
    while (!isEmpty(&stk)){
        doPath(func, elem->fileName, elem->depth);
        elem = pop(&stk);
    }
    printStat(&stats);
    exit(EXIT_SUCCESS);
}

static int function(const char *pathName, int type, int depth) {
    for (int i = 0; i < depth; i++)
        printf(MAGENTA "----| " RESET);
    if (type == FTW_DIR)
        printf(GREEN" %s\n" RESET,  pathName);
    else if (type == FTW_FILE)
        printf(CYAN " %s\n" RESET,  pathName);
    return EXIT_SUCCESS;
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        perror("error: directory is not set\n");
        exit(EXIT_FAILURE);
    }
    ftw(argv[1], function);
    exit(EXIT_SUCCESS);
}

