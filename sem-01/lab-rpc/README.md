# Лабораторная работа №4

## Установить и запустить необходимые пакеты на Ubuntu 22.04.2

1. `sudo apt update`
2. `sudo apt install rpcbind libtirpc-dev`
3. `sudo systemctl start rpcbind` (`sudo systemctl enable rpcbind`), `sudo systemctl status rpcbind`, `rpcinfo`
4. `sudo apt install rpcsvc-proto`, `rpcgen --version`

## Пример из методички

### Создать файл `calculator.x` и сгенерировать серверный и клиентский код с использованием `rpcgen`: `rpcgen -a calculator.x` -> получим 6 файлов (параметр `-a` - сгенерировать все файлы, включая скелетоны для клиента и сервера):

- `calculator.h`: объявление используемых переменных и функций;
- `calculator_xdr.c`: "кодировка" нестандартных типов данных;
- `calculator_clnt.c`: прокси-сервер удаленного вызова, вызывается локальной операционной системой, параметры вызова упакованы в сообщение (пакет). Далее сообщение посылается на сервер. Данный файл генерируется с помощью rpcgen и не нуждается в модификации;
- `calculator_svc.c`: преобразует запрос, вводимый по сети, в вызов локальной процедуры. Файл ответственнен за распаковку полученного сервером сообщения и вызов фактической реализации на стороне сервера и на уровне приложения. Не нуждается в модификации;
- `calculator_client.c`: скелетон программы клиента, требуется модифицировать;
- `calculator_server.c`: скелетон программы сервера, требуется модифицировать;

#### Листинг `calculator.x`:

```/*
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
```

##### Пояснение

Константы `ADD`, `SUB`, `MUL`, `DIV` будут использоваться для указания операции калькулятора (сложение, вычитание, умножение, деление).

Структура `CALCULATOR`, определены поля:

1. `op` — операция (целое число, соответствующее одной из констант: ADD, SUB, MUL, DIV).
2. `arg1`, `arg2` — два аргумента для выполнения операции (типы float).
3. `result` — результат вычисления операции.

RPC программа `CALCULATOR_PROG`: описание RPC программы, которая будет обслуживать удалённые вызовы калькулятора.

1. `CALCULATOR_VER` — это версия программы (в данном случае версия 1).
2. `CALCULATOR_PROC` — это процедура, которая будет обрабатывать структуру CALCULATOR (вход и выход).

Номер программы и версии: программа идентифицируется уникальным номером 0x20000001, а версия программы — номером 1.

#### Модицифированные скелетоны

Клиент взаимодействует с сервером через удалённые вызовы. Код включает:

- Подключение к серверу через RPC.
- Передачу данных (операция, числа).
- Получение результата вычисления.

Сервер обрабатывает запросы от клиента и выполняет необходимые операции. Код включает:

- Обработку структуры CALCULATOR.
- Выполнение операций и возврат результата.

#### Листинг `calculator_client.h`

```#include "calculator.h"
#include <stdio.h>
#include <stdlib.h>

void calculator_prog_1(char *host) {
    CLIENT *clnt;
    struct CALCULATOR *result_1;
    struct CALCULATOR calculator_proc_1_arg;

#ifndef DEBUG
    clnt = clnt_create(host, CALCULATOR_PROG, CALCULATOR_VER, "udp");
    if (clnt == NULL) {
        clnt_pcreateerror(host);
        exit(1);
    }
#endif /* DEBUG */

    // Выбор операции
    char c;
    printf("Choose the operation:\n\t0---ADD\n\t1---SUB\n\t2---MUL\n\t3---DIV\n");
    c = getchar();
    if (c > '3' || c < '0') {
        printf("Error: invalid operation\n");
        exit(1);
    }
    calculator_proc_1_arg.op = c - '0';

    // Ввод чисел
    printf("Input the first number: ");
    scanf("%f", &calculator_proc_1_arg.arg1);
    printf("Input the second number: ");
    scanf("%f", &calculator_proc_1_arg.arg2);

    // Вызов удалённой процедуры
    result_1 = calculator_proc_1(&calculator_proc_1_arg, clnt);
    if (result_1 == (struct CALCULATOR *)NULL) {
        clnt_perror(clnt, "call failed");
        exit(1);
    }

    // Вывод результата
    printf("The Result is %.3f\n", result_1->result);

#ifndef DEBUG
    clnt_destroy(clnt);
#endif /* DEBUG */
}

int main(int argc, char *argv[]) {
    char *host;
    if (argc < 2) {
        printf("Usage: %s server_host\n", argv[0]);
        exit(1);
    }
    host = argv[1];
    calculator_prog_1(host); // Вызов клиента
    exit(0);
}
```

#### Листинг `calculator_server.h`

```#include "calculator.h"

struct CALCULATOR *calculator_proc_1_svc(struct CALCULATOR *argp, struct svc_req *rqstp) {
    static struct CALCULATOR result;

    // Выполнение операции
    switch (argp->op) {
        case ADD:
            result.result = argp->arg1 + argp->arg2;
            break;
        case SUB:
            result.result = argp->arg1 - argp->arg2;
            break;
        case MUL:
            result.result = argp->arg1 * argp->arg2;
            break;
        case DIV:
            if (argp->arg2 != 0) {
                result.result = argp->arg1 / argp->arg2;
            } else {
                printf("Error: Division by zero\n");
                result.result = 0; // Можно обработать ошибку по-другому
            }
            break;
        default:
            printf("Error: Invalid operation\n");
            break;
    }
    return &result;
}
```

### Выполнить сборку проекта с помощью `Makefile.calculator`: `make -f Makefile.calculator`. 

Для корректной сборки необходимо предварительно изменить Makefile.calculator: добавить флаг компиляции -I/usr/include/tirpc и флаг линковки -ltirpc.

#### Листинг `Makefile.calculator`

```# This is a template Makefile generated by rpcgen

# Parameters

CLIENT = calculator_client
SERVER = calculator_server

SOURCES_CLNT.c = 
SOURCES_CLNT.h = 
SOURCES_SVC.c = 
SOURCES_SVC.h = 
SOURCES.x = calculator.x

TARGETS_SVC.c = calculator_svc.c calculator_server.c calculator_xdr.c 
TARGETS_CLNT.c = calculator_clnt.c calculator_client.c calculator_xdr.c 
TARGETS = calculator.h calculator_xdr.c calculator_clnt.c calculator_svc.c calculator_client.c calculator_server.c

OBJECTS_CLNT = $(SOURCES_CLNT.c:%.c=%.o) $(TARGETS_CLNT.c:%.c=%.o)
OBJECTS_SVC = $(SOURCES_SVC.c:%.c=%.o) $(TARGETS_SVC.c:%.c=%.o)
# Compiler flags 

CFLAGS += -g -I/usr/include/tirpc
LDLIBS += -lnsl -ltirpc
RPCGENFLAGS = 

# Targets 

all : $(CLIENT) $(SERVER)

$(TARGETS) : $(SOURCES.x) 
	rpcgen $(RPCGENFLAGS) $(SOURCES.x)

$(OBJECTS_CLNT) : $(SOURCES_CLNT.c) $(SOURCES_CLNT.h) $(TARGETS_CLNT.c) 

$(OBJECTS_SVC) : $(SOURCES_SVC.c) $(SOURCES_SVC.h) $(TARGETS_SVC.c) 

$(CLIENT) : $(OBJECTS_CLNT) 
	$(LINK.c) -o $(CLIENT) $(OBJECTS_CLNT) $(LDLIBS) 

$(SERVER) : $(OBJECTS_SVC) 
	$(LINK.c) -o $(SERVER) $(OBJECTS_SVC) $(LDLIBS)

 clean:
	 $(RM) core $(TARGETS) $(OBJECTS_CLNT) $(OBJECTS_SVC) $(CLIENT) $(SERVER)
```

### Запустить сервер: `./calculator_server`. Запустить клиент: `./calculator_client 127.0.0.1`

![](https://github.com/unaun0/bmstu-os/blob/main/sem-01/lab-rpc/assets/ex1.png)

__RPC являются средством взаимодействия параллельных процессов (IPC - Inter- process communication).__ Два вида взаимодействия параллельных процессов (IPC):
1. Вызов локальных процедур (LPC – local procedure call).
2. Вызов удаленных процедур (RPC – remote procedure call).

__Вызов удаленных процедур сделан максимально подобным вызову локальных процедур__. В ядре RPC не используют протокол, в чем и состоит принципиальное различие с сокетами. Действия фактически выполняются не по протоколу. Действия выполняются в сети путем связывания портов между собой.

## Вопросы к защите

1. Что генерирует rpcЖen? ___client.c, *_server.c, *_svn.c, *_clnt.c, *_xdr.c, *.h__.

2. Структура программы: __генерирует скелетоны для клиента и сервера, два стаба - для клиента и сервера, хедер, xdr - который транслирует ее в bool, и makefile__.

3. Какие файлы скелетоны, какие стабы? ___client.c, *_server.c - скелетоны, а *_svn.c, *_clnt.c - стабы__.

4. Зачем case? __В программе в зависимости от запроса происходит выбор операции__.

5. Что вызывает клиент, чтоб получить номер с буквой? __get_number_1__

6. Где происходит присваивание номера и буквы? __bakery__

7. В одном потоке и если нет, то где? __В нескольких потоках, в bakery_res_1_svc__.

8. С чем вот это связано (тыкая на *_VER в .x файле)? GET_NUMBER присвоить единице, с чем это связано? Для чего здесь присвоили единицу, а здесь двойку? Где в коде? __ТЕОРИЯ: GET_NUMBER - константа, применяемая в свитче (который в файле *_svc.c), используется для того, чтобы определить действие программы. А именно, get_number - функция, которая назначает клиенту номер__.

9. Что реализуем с помощью очереди? __Взаимное исключение__.

10. Почему именно такое содержимое в bakery.x? __Объявляем константы, структуры, функции для общения клиента с сервером (зависит от реализации)__.
