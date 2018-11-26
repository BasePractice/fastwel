#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "resource.h"

static char memory_suffix[] = {
        'B', 'K', 'M', 'G', 'T'
};


#if defined(WIN32)

#include <Windows.h>
#include <psapi.h>

void pick_process_statistic() {
    HANDLE h_process = GetCurrentProcess();
    DWORD process_id = GetProcessId(h_process);
    DWORD opened_handle = 0;
    IO_COUNTERS io_counters;
    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    SYSTEMTIME system_time;
    int i_memory;
    size_t memory_usage;
    time_t seconds = 0L;

    memset(&io_counters, 0, sizeof(io_counters));
    GetProcessHandleCount(h_process, &opened_handle);
    GetProcessIoCounters(h_process, &io_counters);
    GetProcessTimes(h_process, &creation_time, &exit_time, &kernel_time, &user_time);
    FileTimeToSystemTime(&creation_time, &system_time);

    {
        PROCESS_MEMORY_COUNTERS_EX pmc;
        size_t bytes;

        GetProcessMemoryInfo(GetCurrentProcess(), (PPROCESS_MEMORY_COUNTERS) &pmc, sizeof(pmc));
        memory_usage = bytes = pmc.PrivateUsage;
        for (i_memory = 0;
             i_memory < sizeof(memory_suffix) / sizeof(memory_suffix[0]) && bytes >= 1024; ++i_memory, bytes /= 1024) {
            memory_usage = (size_t) (bytes / 1024.0);
        }
    }

    {
        time(&seconds);
    }

    fprintf(stdout, "TIME: %010llu, HNDL: %010lu, IO/R: %010llu, IO/W: %010llu, IO/O: %010llu, MEMO: %09llu%c\n",
            seconds, opened_handle,
            io_counters.ReadOperationCount, io_counters.WriteOperationCount, io_counters.OtherOperationCount,
            memory_usage, memory_suffix[i_memory]);
    fflush(stdout);
}

#else

#include <string.h>
#include <stdio.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>


void pick_process_statistic() {

    int i_memory = 0;
    size_t memory_usage = 0;
    size_t bytes = 0;
    time_t seconds = 0L;
    size_t handles = 0;
    static filename_hdl[64] = {0};
    static filename_stat[64] = {0};

    {
        struct dirent *dp;
        DIR *dir;


        if (filename_hdl[0] == 0) {
            sprintf(filename_hdl, "/proc/%d/fd/", getpid());
        }
        if (filename_stat[0] == 0) {
            sprintf(filename_stat, "/proc/%d/stat", getpid());
        }

        dir = opendir(filename_hdl);
        while ((dp = readdir(dir)) != NULL) {
            handles++;
        }
        closedir(dir);

        FILE *fd = fopen(filename_stat, "rt");
        if (fd != 0) {
            char number[20];
            int  number_it = 0;
            int  param_it = 0;

            while (!feof(fd) && number_it < sizeof(number)) {
                int ch = fgetc(fd);
                if (ch == -1)
                    break;
                if (ch == ' ') {
                    number[number_it] = 0;
                    if (param_it == 22) {
                        bytes = (size_t)strtol(number, 0, 10);
                        break;
                    }
                    number_it = 0;
                    ++param_it;
                } else {
                    number[number_it++] = (char)ch;
                }
            }
            fclose(fd);
        }

        memory_usage = bytes;
        for (i_memory = 0;
             i_memory < sizeof(memory_suffix) / sizeof(memory_suffix[0]) && bytes >= 1024; ++i_memory, bytes /= 1024) {
            memory_usage = (size_t) (bytes / 1024.0);
        }
    }

    {
        time(&seconds);
    }


    fprintf(stdout, "TIME: %010llu, HNDL: %010lu, IO/R: %010llu, IO/W: %010llu, IO/O: %010llu, MEMO: %09llu%c\n",
            (unsigned long long)seconds, handles, 0LL, 0LL, 0LL, (unsigned long long)memory_usage, memory_suffix[i_memory]);
    fflush(stdout);
}

#endif

#if defined(MEMORY_LEAK_DETECT)

void *
u_allocate_dbg(const char *file_name, int line, size_t size) {
    void *ret = calloc(1, size);
    fprintf(stdout, "[allocate 0x%p] %s:%d\n", ret, file_name, line);
    fflush(stdout);
    return ret;
}

void *
u_realloc_dbg(const char *file_name, int line, void *p_memory, size_t size) {
    void *ret = realloc(p_memory, size);
    fprintf(stdout, "[realloc  0x%p] %s:%d\n", ret, file_name, line);
    fflush(stdout);
    return ret;
}

void
u_free_dbg(const char *file_name, int line, void **p_memory) {
    if (0 != p_memory && 0 != (*p_memory)) {
        fprintf(stdout, "[free     0x%p] %s:%d\n", (*p_memory), file_name, line);
        fflush(stdout);
        free((*p_memory));
        (*p_memory) = 0;
    }
}

#else
void *
u_allocate(size_t size) {
    return calloc(size, 1);
}

void *
u_realloc(void *p_memory, size_t size) {
    return realloc(p_memory, size);
}

void
u_free(void **p_memory) {
    if (0 != p_memory && 0 != (*p_memory)) {
        free( (*p_memory) );
        (*p_memory) = 0;
    }
}
#endif
