struct REQUEST {
    int idx;
    char type;
    char result;
};

program PRODCONS_PROG {
    version PRODCONS_VERS {
        REQUEST service(REQUEST) = 1;
    } = 1;
} = 0x20000001;

