#include <windows.h>
#include <stdio.h>
#include <signal.h>
#include <stdbool.h>
#include <time.h>

#define READERS 5
#define WRITERS 3

#define MIN_SLEEP_READERS 1200
#define MIN_SLEEP_WRITER 1300
#define MAX_SLEEP_READERS 1500
#define MAX_SLEEP_WRITER 1700

HANDLE can_read;
HANDLE can_write;
HANDLE mutex;

LONG waiting_writers = 0;
LONG waiting_readers = 0;
LONG active_readers = 0;
bool writing = false;

HANDLE readers_threads[READERS];
HANDLE writers_threads[WRITERS];

int readers_id[READERS];
int writers_id[WRITERS];

char ch = 0;
int flag = 1;

void start_read() {
	InterlockedIncrement(&waiting_readers);
	if (writing || (WaitForSingleObject(can_write, 0) == WAIT_OBJECT_0))
		WaitForSingleObject(can_read, INFINITE);
	WaitForSingleObject(mutex, INFINITE);
	InterlockedDecrement(&waiting_readers);
	InterlockedIncrement(&active_readers);
	SetEvent(can_read);
	ReleaseMutex(mutex);
}

void stop_read() {
	InterlockedDecrement(&active_readers);
	if (!active_readers)
		SetEvent(can_write);
}

DWORD WINAPI reader(CONST LPVOID param) {
	int id = *(int *)param;
	srand(id);
	time_t sleep_time;
	while(flag) {
		sleep_time = rand() % (MAX_SLEEP_READERS - MIN_SLEEP_READERS) + MIN_SLEEP_READERS;
		start_read();
		printf("reader %d: %c (sleep time = %d)\n", id, 'a' + (ch - 1) % 26, sleep_time);
		stop_read();
		Sleep(sleep_time);
	}
}

void start_write() {
	InterlockedIncrement(&waiting_writers);
	if (active_readers > 0 || writing)
		WaitForSingleObject(can_write, INFINITE);
	InterlockedDecrement(&waiting_writers);
	writing = true;
}

void stop_write() {
	writing = false;
	if (waiting_readers)
		SetEvent(can_read);
	else
		SetEvent(can_write);
}

DWORD WINAPI writer(CONST LPVOID param) {
	int id = *(int *)param;
	srand(id);
	time_t sleep_time;
	while(flag) {
		sleep_time = rand() % (MAX_SLEEP_WRITER - MIN_SLEEP_WRITER) + MIN_SLEEP_WRITER;
		start_write();
        char c = 'a' + (ch % 26);
        ++ch;
		printf("writer %d: %c (sleep time = %d)\n", id, c, sleep_time);
		stop_write();
		Sleep(sleep_time);
	}
}

void sig_catch(int sig_num) {
    flag = 0;
    printf("signal %d caught\n", sig_num);
}

int main(void) {
	signal(SIGINT, sig_catch);
	setbuf(stdout, NULL);
	if ((mutex = CreateMutex(NULL, FALSE, NULL)) == NULL) {
		perror("CreateMutex");
		exit(1);
	}
	if ((can_read = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		perror("CreateEvent (can_read)");
		exit(1);
	}
	if ((can_write = CreateEvent(NULL, FALSE, FALSE, NULL)) == NULL) {
		perror("CreateEvent (can_write)");
		exit(1);
	}
	DWORD id = 0;
	for (size_t i = 0; i < WRITERS; ++i) {
		writers_id[i] = i;
		if ((writers_threads[i] = CreateThread(NULL, 0, &writer, writers_id + i, 0, &id)) == NULL) {
			perror("CreateThread (writer)");
			exit(1);
		}
	}
    for (size_t i = 0; i < READERS; ++i) {
		readers_id[i] = i;
		if ((readers_threads[i] = CreateThread(NULL, 0, &reader, readers_id + i, 0, &id)) == NULL) {
			perror("CreateThread (reader)");
			exit(1);
		}
	}
	WaitForMultipleObjects(WRITERS, writers_threads, TRUE, INFINITE);
    WaitForMultipleObjects(READERS, readers_threads, TRUE, INFINITE);
	for (size_t i = 0; i < WRITERS; ++i)
		CloseHandle(writers_threads[i]);
	for (size_t i = 0; i < READERS; ++i)
		CloseHandle(readers_threads[i]);
	CloseHandle(can_read);
	CloseHandle(can_write);
	CloseHandle(mutex);

	exit(0);
}