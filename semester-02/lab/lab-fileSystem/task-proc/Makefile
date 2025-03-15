CC = gcc
CFLAGS = -Wall -std=c99 -I.

EXEC_TASK_01 = task-01/app
EXEC_MAIN = proc_info
EXEC_PAGEMAP = pagemap

all: $(EXEC_TASK_01) $(EXEC_MAIN) $(EXEC_PAGEMAP)

$(EXEC_TASK_01): task-01/main.c
	$(CC) $(CFLAGS) -o $(EXEC_TASK_01) task-01/main.c

$(EXEC_MAIN): proc_info.c
	$(CC) $(CFLAGS) -o $(EXEC_MAIN) proc_info.c
	
$(EXEC_PAGEMAP): pagemap.c
	$(CC) $(CFLAGS) -o $(EXEC_PAGEMAP) pagemap.c
clean:
	rm -f $(EXEC_TASK_01) $(EXEC_MAIN) $(EXEC_PAGEMAP)

