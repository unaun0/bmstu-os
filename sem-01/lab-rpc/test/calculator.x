/*
 * filename: calculator.x
 * function: Define constants, non-standard data types and the calling process in remote calls
 */

const ADD = 0;
const SUB = 1;
const MUL = 2;
const DIV = 3;

struct CALCULATOR
{
    int op;        /* Операция: ADD, SUB, MUL, DIV */
    float arg1;    /* Первый аргумент */
    float arg2;    /* Второй аргумент */
    float result;  /* Результат */
};

program CALCULATOR_PROG
{
    version CALCULATOR_VER
    {
        struct CALCULATOR CALCULATOR_PROC(struct CALCULATOR) = 1;
    } = 1; /* Версия интерфейса */
} = 0x20000001; /* Номер программы RPC */