#define _GNU_SOURCE
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <fcntl.h>
#include <dirent.h>
#include <limits.h>
#include <sys/types.h>
#include "const.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PATH_MAX 4096
#define PAGE_SIZE 4096

int get_pid(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "error: PID must be provided as a command-line argument\n");
        exit(EXIT_FAILURE);
    }
    int pid = atoi(argv[1]);
    if (pid <= 0) {
        fprintf(stderr, "error: invalid PID: %d.\n", pid);
        exit(EXIT_FAILURE);
    }
    char check_dir[PATH_MAX] = {'\0'};
    snprintf(check_dir, PATH_MAX, "/proc/%d", pid);
    if (access(check_dir, F_OK) != 0) {
        perror("error: process not found or unable to access the /proc directory");
        exit(EXIT_FAILURE);
    }
    return pid;
}

void print_cmdline(const int pid) {
    char path[PATH_MAX] = {'\0'};
    snprintf(path, sizeof(path), "/proc/%d/cmdline", pid);
    FILE *file = fopen(path, "r");
    if (file == NULL) {
        perror("error opening cmdline file");
        return;
    }
    char buf[PATH_SIZE] = {'\0'};
    size_t len = fread(buf, 1, sizeof(buf) - 1, file);
    if (len == 0) {
        fprintf(stderr, "error reading cmdline for PID %d\n", pid);
        fclose(file);
        return;
    }
    buf[len] = '\0';
    printf("CMDLINE:%s\n", buf);
    fclose(file);
}

void print_cwd(const int pid) {
    char pathToOpen[PATH_MAX] = {'\0'};
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/cwd", pid);
    char buf[PATH_SIZE] = {'\0'};
    ssize_t len = readlink(pathToOpen, buf, sizeof(buf) - 1);
    if (len == -1) {
        perror("error reading cwd link");
        return;
    }
    buf[len] = '\0';
    printf("CWD:%s\n", buf);
}

void print_comm(const int pid) {
    char pathToOpen[PATH_MAX] = {'\0'};
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/comm", pid);
    FILE *file = fopen(pathToOpen, "r");
    if (file == NULL) {
        perror("error opening comm file");
        return;
    }
    char buf[PATH_SIZE] = {'\0'};
    size_t lengthOfRead = fread(buf, 1, sizeof(buf) - 1, file);
    if (lengthOfRead == 0) {
        fprintf(stderr, "error reading comm for PID %d\n", pid);
        fclose(file);
        return;
    }
    buf[lengthOfRead] = '\0';
    printf("COMM:%s\n", buf);
    fclose(file);
}

void print_exe(const int pid) {
    char pathToOpen[PATH_MAX] = {'\0'};
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/exe", pid);
    char buf[PATH_SIZE] = {'\0'};
    ssize_t len = readlink(pathToOpen, buf, sizeof(buf) - 1);
    if (len == -1) {
        perror("error reading exe link");
        return;
    }
    buf[len] = '\0';
    printf("EXE:%s\n", buf);
}

void print_environ(const int pid) {
    char pathToOpen[PATH_MAX] = {'\0'};
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/environ", pid);
    FILE *file = fopen(pathToOpen, "r");
    if (file == NULL) {
        perror("Error opening environ file");
        return;
    }
    char buf[PATH_SIZE] = {'\0'};
    printf("ENVIRON:\n");
    size_t len;
    while ((len = fread(buf, 1, sizeof(buf) - 1, file)) > 0) {
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == '\0')
                buf[i] = '\n';
        }
        buf[len] = '\0';
        for (size_t i = 0; i < len; i++) {
            if (buf[i] == '\n')
                printf("\t");
            printf("%c", buf[i]);
        }
    }
    printf("\n");
    fclose(file);
}

void print_fd(const int pid) {
    char pathToOpen[PATH_MAX] = {'\0'};
    char string[PATH_MAX] = {'\0'};
    char path[PATH_SIZE] = {'\0'};
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/fd/", pid);
    DIR *dir = opendir(pathToOpen);
    if (dir == NULL) {
        perror("failed to open directory");
        return;
    }
    printf("FD:\n");
    struct dirent *readDir;
    while ((readDir = readdir(dir)) != NULL) {
        if (strcmp(readDir->d_name, ".") == 0 || strcmp(readDir->d_name, "..") == 0)
            continue;
        size_t path_len = strlen(pathToOpen) + strlen(readDir->d_name) + 1;
        if (path_len >= sizeof(path)) {
            fprintf(stderr, "warning: Path is too long for buffer\n");
            continue;
        }
        snprintf(path, sizeof(path), "%s%s", pathToOpen, readDir->d_name);
        ssize_t len = readlink(path, string, sizeof(string) - 1);
        if (len == -1) {
            perror("failed to read link");
            continue;
        }
        string[len] = '\0';
        printf("%s -> %s\n", readDir->d_name, string);
    }
    printf("\n");
    closedir(dir);
}

void print_io(const int pid) {
    char pathToOpen[PATH_MAX] = {'\0'};
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/io", pid);
    FILE *file = fopen(pathToOpen, "r");
    if (file == NULL) {
        perror("failed to open IO file");
        return;
    }
    char buf[PATH_SIZE] ;
    size_t lengthOfRead;
    printf("IO:\n");
    while ((lengthOfRead = fread(buf, 1, sizeof(buf) - 1, file)) > 0) {
        buf[lengthOfRead] = '\0';
        printf("%s", buf);
    }
    printf("\n");
    if (ferror(file))
        perror("Error reading IO file");
    fclose(file);
}

typedef struct {
    unsigned long start_addr;
    unsigned long end_addr;
} MemoryRegion;

void print_region(int mem_fd, unsigned long start_addr, unsigned long end_addr, FILE *stream) {
    size_t region_size = end_addr - start_addr;
    void *buffer = malloc(region_size);
    if (!buffer) {
        perror("malloc");
        return;
    }

    if (lseek(mem_fd, start_addr, SEEK_SET) == (off_t)-1) {
        perror("lseek");
        free(buffer);
        return;
    }

    ssize_t bytes_read = read(mem_fd, buffer, region_size);
    if (bytes_read == -1) {
        perror("read");
        free(buffer);
        return;
    }

    fprintf(stream, "Memory region: %lx - %lx\n", start_addr, end_addr);
    for (size_t i = 0; i < region_size; i++) {
        fprintf(stream, "%02x ", ((unsigned char *)buffer)[i]);
        if ((i + 1) % 16 == 0) fprintf(stream, "\n");
    }
    fprintf(stream, "\n");
    free(buffer);
}

MemoryRegion *get_memory_regions(int pid, size_t *region_count) {
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%d/maps", pid);
    FILE *file = fopen(path, "r");
    if (!file) {
        perror("fopen");
        return NULL;
    }

    size_t capacity = 10;
    MemoryRegion *regions = malloc(capacity * sizeof(MemoryRegion));
    if (!regions) {
        perror("malloc");
        fclose(file);
        return NULL;
    }

    char *line = NULL;
    size_t line_size = 0;
    size_t count = 0;
    while (getline(&line, &line_size, file) != -1) {
        unsigned long start_addr, end_addr;
        if (sscanf(line, "%lx-%lx", &start_addr, &end_addr) == 2) {
            if (count >= capacity) {
                capacity *= 2;
                MemoryRegion *new_regions = realloc(regions, capacity * sizeof(MemoryRegion));
                if (!new_regions) {
                    perror("realloc");
                    free(regions);
                    fclose(file);
                    free(line);
                    return NULL;
                }
                regions = new_regions;
            }
            regions[count].start_addr = start_addr;
            regions[count].end_addr = end_addr;
            count++;
        }
    }
    fclose(file);
    free(line);
    *region_count = count;
    return regions;
}

void print_mem(int pid, FILE *stream) {
    printf("\nMEM:\n");
    size_t region_count = 0;
    MemoryRegion *regions = get_memory_regions(pid, &region_count);
    if (!regions) {
        fprintf(stderr, "Failed to get memory regions\n");
        return;
    }

    char mem_path[PATH_MAX];
    snprintf(mem_path, PATH_MAX, "/proc/%d/mem", pid);
    int mem_fd = open(mem_path, O_RDONLY);
    if (mem_fd == -1) {
        perror("open");
        free(regions);
        return;
    }

    for (size_t i = 0; i < region_count; i++) {
        fprintf(stream, "Region %zu: %lx - %lx\n", i, regions[i].start_addr, regions[i].end_addr);
        print_region(mem_fd, regions[i].start_addr, regions[i].end_addr, stream);
    }

    close(mem_fd);
    free(regions);
}

void run_pagemap(int pid, unsigned long start_addr) {
    char command[100];
    snprintf(command, sizeof(command), "./pagemap %d 0x%lx", pid, start_addr);
    int status = system(command);
    if (status == -1) {
        perror("system() failed");
    }
}

void run_pagemap_region(int pid, unsigned long start_addr, unsigned long end_addr) {
    printf("Fetching PAGEMAP info for PID %d, region: 0x%lx-0x%lx\n", pid, start_addr, end_addr);
    for (unsigned long addr = start_addr; addr < end_addr; addr += PAGE_SIZE) {
        run_pagemap(pid, addr);
        break;
    }
}

void print_page(uint64_t address, uint64_t data, FILE *out) {
    fprintf(out, "0x%-16lx : %-16lx %-10ld %-10ld %-10ld %-10ld\n", address,
            data & (((uint64_t)1 << 55) - 1), (data >> 55) & 1,
            (data >> 61) & 1, (data >> 62) & 1, (data >> 63) & 1);
}

void print_pagemap(const char *proc, FILE *out) {
    fprintf(out, "PAGEMAP\n");
    fprintf(out, " addr : pfn soft-dirty file/shared swapped present\n");
    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "/proc/%s/maps", proc);
    FILE *maps = fopen(path, "r");
    snprintf(path, PATH_MAX, "/proc/%s/pagemap", proc);
    int pm_fd = open(path, O_RDONLY);
    char buf[PATH_SIZE + 1] = "\0";
    int len;
    while ((len = fread(buf, 1, PATH_SIZE, maps)) > 0) {
        for (int i = 0; i < len; ++i)
            if (buf[i] == 0) buf[i] = '\n';
        buf[len] = '\0';
        char *save_row;
        char *row = strtok_r(buf, "\n", &save_row);
        while (row) {
            char *addresses = strtok(row, " ");
            char *start_str, *end_str;
            if ((start_str = strtok(addresses, "-")) && (end_str = strtok(NULL, "-"))) {
                uint64_t start = strtoul(start_str, NULL, 16);
                uint64_t end = strtoul(end_str, NULL, 16);
                for (uint64_t i = start; i < end; i += sysconf(_SC_PAGE_SIZE)) {
                    uint64_t offset;
                    uint64_t index = i / sysconf(_SC_PAGE_SIZE) * sizeof(offset);
                    pread(pm_fd, &offset, sizeof(offset), index);
                    print_page(i, offset, out);
                }
            }
            row = strtok_r(NULL, "\n", &save_row);
        }
    }
    fclose(maps);
    close(pm_fd);
}

#define MAX_REGIONS 100

void print_maps(int pid) {
    char *line = NULL;
    size_t line_size = 0;
    ssize_t line_length;
    char pathToMaps[PATH_MAX];
    char pathToMem[PATH_MAX];
    snprintf(pathToMaps, sizeof(pathToMaps), "/proc/%d/maps", pid);
    snprintf(pathToMem, sizeof(pathToMem), "/proc/%d/mem", pid);
    FILE *maps_file = fopen(pathToMaps, "r");
    if (!maps_file) {
        perror("Failed to open /proc/[pid]/maps");
        exit(EXIT_FAILURE);
    }
    int mem_fd = open(pathToMem, O_RDONLY);
    if (mem_fd == -1) {
        perror("Failed to open /proc/[pid]/mem");
        fclose(maps_file);
        exit(EXIT_FAILURE);
    }
    MemoryRegion regions[100];
    int region_count = 0;
    unsigned long start_addr, end_addr;
    char perms[5];
    unsigned int offset;
    char dev[6];
    int inode;
    char file_path[PATH_MAX];
    printf("\nMAPS:\n");
    while ((line_length = getline(&line, &line_size, maps_file)) != -1) {
    if (line_length == 0) continue;
    file_path[0] = '\0';

    int fields_read = sscanf(line, "%lx-%lx %4s %x %5s %d %s",
                             &start_addr, &end_addr, perms, &offset, dev, &inode, file_path);
    if (fields_read >= 6) {
        size_t region_size = end_addr - start_addr;
        size_t pages = region_size / sysconf(_SC_PAGESIZE);
        if (fields_read == 6) {
            printf("%8zu %lx-%lx %s %x %s %d\n",
                   pages, start_addr, end_addr, perms, offset, dev, inode);
        } else {
            printf("%8zu %s", pages, line);
        }
        if (fields_read > 6 && strstr(file_path, "/home/parallels/Desktop/proc_info_2/task-03/s.out") != NULL) {
            if (region_count < 100) {
                regions[region_count].start_addr = start_addr;
                regions[region_count].end_addr = end_addr;
                region_count++;
            }
        }
    }
}
    if (ferror(maps_file)) {
        perror("Error reading /proc/[pid]/maps");
        fclose(maps_file);
        close(mem_fd);
        free(line);
        exit(EXIT_FAILURE);
    }
    fclose(maps_file);
    free(line);
    printf("\nSelected memory regions:\n");
    for (int i = 0; i < region_count; i++) {
        print_region(mem_fd, regions[i].start_addr, regions[i].end_addr, stdout);
    }
    close(mem_fd);
}


void print_root(const int pid) {
    char pathToOpen[PATH_MAX];
    char buf[PATH_MAX];
    snprintf(pathToOpen, sizeof(pathToOpen), "/proc/%d/root", pid);
    ssize_t len = readlink(pathToOpen, buf, sizeof(buf) - 1);
    if (len == -1) {
        perror("failed to read root directory link");
        return;
    }
    buf[len] = '\0';
    printf("\nROOT:%s\n", buf);
}

void print_stat(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/stat", pid);
    char buf[PATH_SIZE];
    FILE *file = fopen(pathToOpen, "r");
    fread(buf, 1, PATH_SIZE, file);
    char *token = strtok(buf, " ");
    printf("\nSTAT:\n");
    for (int i = 0; token != NULL; i++) {
        printf(NO_DESCR[i], token);
        token = strtok(NULL, " ");
    }
    fclose(file);
}

void print_statm(const int pid) {
    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/statm", pid);
    FILE *file = fopen(pathToOpen, "r");
    char buf[PATH_SIZE];
    fread(buf, 1, PATH_SIZE, file);

    char *token = strtok(buf, " ");
    printf("\nSTATM: \n");
    for (int i = 0; token != NULL; i++) {
        printf(STATM_PATTERNS[i], token);
        token = strtok(NULL, " ");
    }
    fclose(file);
}
void print_task(const int pid) {
    DIR *d;
    struct dirent *dir;

    char pathToOpen[PATH_MAX];
    snprintf(pathToOpen, PATH_MAX, "/proc/%d/task", pid);
    printf("\nTASK:\n");
    d = opendir(pathToOpen);
    if (d) {
        while ((dir = readdir(d)) != NULL)
            printf("%s\n", dir->d_name);
        closedir(d);
    }
}

int main(int argc, char *argv[]) {
    FILE *out = stdout;
    int pid = get_pid(argc, argv);
    print_cmdline(pid);
    print_cwd(pid);
    print_comm(pid);
    print_exe(pid);
    print_environ(pid);
    print_fd(pid);
    print_io(pid);
    print_maps(pid);
    // print_mem(pid, out);
    print_pagemap(argv[1], out);
    print_root(pid);
    print_stat(pid);
    print_statm(pid);
    print_task(pid);
    
    return EXIT_SUCCESS;
}
