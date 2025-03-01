# Сетевые сокеты 

Сетевые сокеты используются для обмена данными между процессами по сети. В Linux они реализуются с помощью системных вызовов из библиотеки <sys/socket.h>. 

## 1. Создание сокета - `socket()`

```
int sock_fd = socket(AF_INET, SOCK_STREAM, 0);
```

* `AF_INET` — семейство адресов IPv4 (можно использовать AF_INET6 для IPv6).
* `SOCK_STREAM` — потоковый сокет (TCP). Если нужен UDP, то SOCK_DGRAM.
* `0` — протокол (по умолчанию для TCP/UDP).

Возвращает файловый дескриптор (FD) сокета или `-1` в случае ошибки.

## 2. Что такое `struct sockaddr_in`?

Это структура, которая хранит информацию об IP-адресе (IP-адрес - 32 бита), порте (номер порта - 16 бит ) и о семействе адресов (`AF_INET` - IPv4). 

**Альтернативы `sin_family`:** `AF_INET6` — для IPv6; `AF_UNIX` — для локального межпроцессного взаимодействия.

**Указание IP-адреса (`sin_addr`):** `server_addr.sin_addr.s_addr = INADDR_ANY;`

`INADDR_ANY` (значение `0.0.0.0`) означает, что сервер будет принимать соединения на всех доступных интерфейсах.

**Указание порта - `htons(SERVER_PORT)`:** `server_addr.sin_port = htons(SERVER_PORT);`

SERVER_PORT — номер порта, на котором работает сервер.

📌 `htons()` (*host to network short*) переводит 16-битный порт в сетевой порядок байтов (*big-endian*).

📌 Важно, потому что разные архитектуры процессоров могут использовать разные порядки байтов.

Она используется в таких системных вызовах, как `bind()`, `connect()` и `accept()`.

## 3. Привязка сокета к адресу - `bind()` - для сервера

```
bind(server_sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr))
```

**Серверу нужно знать, на каком IP и порту слушать входящие соединения.**

## 4. Ожидание подключений - `listen()` - для сервера

```
listen(server_sock_fd, SERVER_BACKLOG)
```

* `server_sock_fd` — файловый дескриптор сокета.
* `SERVER_BACKLOG` — максимальная очередь ожидающих подключений.

**Этот вызов переводит сокет в режим пассивного ожидания соединений.**

## 5. Принятие входящего соединения - `accept()` - для сервера

```
struct sockaddr_in client_addr;
socklen_t client_len = sizeof(client_addr);
int client_sock_fd = accept(server_sock_fd, (struct sockaddr*)&client_addr, &client_len);
```

* `accept()` - блокирует выполнение до появления нового клиента.

* Возвращает новый файловый дескриптор (`client_sock_fd`) для общения с клиентом.

## 6. Подключение клиента к серверу - `connect()` - для клиента

```
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));

server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(SERVER_PORT);
inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr); 

if (connect(sock_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
    perror("connect");
    close(sock_fd);
    exit(EXIT_FAILURE);
}
```

`inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr)` — переводит IP-адрес в бинарный формат.

Клиент использует `connect()` для установления TCP-соединения с сервером.

## 7. Отправка и получение данных

### 7.1. Отправка данных - `send()`

```
char message[] = "Hello, Server!";
send(client_sock_fd, message, strlen(message), 0)
```

`send(сокет, буфер, размер, флаги)`

Возвращает количество отправленных байтов или `-1` в случае ошибки.

### 7.2. Прием данных - `recv()`

```
char buffer[256];
memset(buffer, 0, sizeof(buffer));
recv(client_sock_fd, buffer, sizeof(buffer), 0)
printf("Получено сообщение: %s\n", buffer);
```

`recv(сокет, буфер, размер, флаги)`

Возвращает количество байтов или `0`, если соединение закрыто.

## 8. Закрытие сокетов - `close()`

```
close(client_sock_fd);
close(server_sock_fd);
```

* Сервер закрывает `client_sock_fd` после общения с клиентом.
* `server_sock_fd` закрывается при завершении работы сервера.

Сервер закрывает client_sock_fd после общения с клиентом.
server_sock_fd закрывается при завершении работы сервера.

# 🔹 Общий алгоритм работы

## Для сервера

* Создает сокет (`socket`).
* Привязывает его к IP/порту (`bind`).
* Слушает входящие соединения (`listen`).
* Принимает первое соединение из очереди входищий (`accept`).
* Принимает данные (`recv`) и отправляет ответ (`send`).
* Закрывает соединение (`close`).

## Для клиента

* Создает сокет (`socket`).
* Подключается к серверу (`connect`).
* Отправляет запрос (`send`).
* Получает ответ (`recv`).
* Закрывает соединение (`close`).

## 🔹 Как это работает в коде?

Cервер ждет подключения клиентов и создает для каждого дочерний процесс (или поток).

**💡 Важно**: в многопоточной версии `fork()` заменяется на `pthread_create()` - `detach`, чтобы освобождать ресурсы автоматически.

