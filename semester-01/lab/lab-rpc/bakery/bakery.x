struct REQUEST
{
    int pid;
    int idx;
    int num;
    int op;
    float arg1;
    float arg2;
    float res;
};

program BAKERY_PROG
{
    version BAKERY_VER
    {
        int GET_NUMBER(struct REQUEST) = 1;
        float BAKERY_SERVICE(struct REQUEST) = 2;
    } = 1; /* Version number = 1 */
} = 0x20000001; /* RPC program number */