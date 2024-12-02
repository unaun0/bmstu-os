struct REQUEST {
    int op;
    int pid;
    int idx;
    int res;
};

program PRODCONS_PROG {
    version PRODCONS_VERS {
        int PRODUCER(REQUEST) = 1;
        int CONSUMER(REQUEST) = 2;
    } = 1;
} = 0x20000001;

