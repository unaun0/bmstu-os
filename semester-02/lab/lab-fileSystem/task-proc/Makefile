CC = gcc
CFLAGS = -Wall -std=c99 -I.

EXEC_MAIN = proc_info
EXEC_PAGEMAP = pagemap

all: $(EXEC_TASK_01) $(EXEC_MAIN) $(EXEC_PAGEMAP)

$(EXEC_MAIN): proc_info.c
	$(CC) $(CFLAGS) -o $(EXEC_MAIN) proc_info.c
	
$(EXEC_PAGEMAP): pagemap.c
	$(CC) $(CFLAGS) -o $(EXEC_PAGEMAP) pagemap.c
clean:
	rm -f $(EXEC_MAIN) $(EXEC_PAGEMAP)


