# Содержание

1. [Лабораторная работа №1](#lab1)  
    1. 1. [Задание 1](#lab1_1)  
    1. 2. [Задание 2](#lab1_2)  
    1. 3. [Задание 3](#lab1_3)  
    1. 4. [Задание 4](#lab1_4)
2. [Лабораторная работа №2](#lab2)  
    1. 1. [Задание 1](#lab2_1)  
    1. 2. [Задание 2](#lab2_2)  
    1. 3. [Задание 3](#lab2_3)  
    1. 4. [Задание 4](#lab2_4)
    1. 5. [Задание 5](#lab2_5)
    1. 5. [Задание 6](#lab2_6)
3. [Типы файлов и права доступа](#file_type)
4. [Команда `ps`](#ps)
5. [Состояния процессов](#state)
6. [Флаги процессов](#flags)
7. [Файловая система UNIX, inode](#file_UNIX)
8. [Ссылки](#links)
9. [Программный канал](#pipe)
10. [Процесс](#proc)
11. [`fork()`](#fork)
12. [`wait()`](#wait)
13. [`exec()`](#exec)
14. [Сигнал](#signal)
14. [Демон](#daemon)

<a name="lab1"><h1>Лабораторная работа №1</h1></a>

<a name="lab1_1"><h2> Задание 1</h2></a>

Используя команду `mkdir`, cоздайте директорию (например, `mkdir iu7-54b`). Перейдите в созданную директорию с помощью команды `cd` (например, `cd iu7-54b`). Создайте поддиректорию (например, используя свою фамилию, `mkdir tskhovrebova`). 

#### Команда `ls`
Выводит список файлов.

```
tsyar@MacBook-Pro-Chovrebova iu7-54b % ls
tskhovrebova
```

- `-a` -- в том числе - скрытые.

- `-l` -- в несколько столбцов с дополнительными данными.

```
tsyar@MacBook-Pro-Chovrebova iu7-54b % ls -l
total 0
drwxr-xr-x  2 tsyar  staff  64 23 сен 00:03 tskhovrebova
```
<a name="file_type"></a>

`d` | `rwx` | `r-x` | `r-x`_ -- тип файла и права доступа.
`2` -- количество жестких ссылок на файл.
`tsyar` -- владелец файла.
`staff` -- группа владельца.
`64` -- размер файла в байтах.
`23 сен 00:03` -- дата и время создания файла.
`tskhovrebova` -- имя файла.

Существует __7 типов файлов__:
1. `d` -- справочник или директория.
2. `-` -- обычный файл.
3. `l` -- символическая связь (канал) – символьная ссылка.
4. `b` -- специальный блочный файл - блочное устройство.
5. `c` -- специальный символьный файл - символьное устройство 
6. `p` -- программный канал (pipe)
7. `s` -- сокет в файловом пространстве имен (Домен – UNIX)

- `r` -- права на чтение.
- `w` -- права на запись.
- `x` -- права на выполнение.

Права доступа разделяются на __3 части__:
1. Следующие три символа -- __права доступа для владельца__ (в данном случае, `rwx`).
2. Следующие три символа -- __права доступа для группы__ (в данном случае, `r-x`).
3. Следующие три символа -- __права доступа для всех остальных__ (в данном случае, `r-x`).

- `-i` -- порядковый номер файла в файловой системе.

```
tsyar@MacBook-Pro-Chovrebova iu7-54b % ls -i
6240049 tskhovrebova
```
<a name="ps"></a>
#### Команда `ps`

Просмотр информации о процессах, запущенных в системе.

- `-e` -- отображает информацию о всех процессах в системе, включая процессы, запущенные другими пользователями.

- `-a` -- отображает информацию о всех процессах, запущенных на терминале.

- `-f` -- выводит информацию о процессах в более подробном виде.

- `-x` -- отображает информацию о процессах, которые не имеют контролирующего терминала (например, демоны).

```
UID   PID  PPID   C STIME   TTY           TIME CMD
501 17384 17377   0  1:41   ttys001    0:00.14 /bin/zsh -il
501   384     1   0 пт02  ??         1:49.54 /usr/sbin/distnoted agent
```

1. __PID__ (идентификатор процесса)
2. __TTY__ (терминал, на котором запущен процесс)
3. __TIME__ (время, затраченное процессом)
4. __CMD__ (команда, которая запустила процесс)
5. __PPID__(идентификатор родительского процесса)
6. __UID__ (идентификатор пользователя, запустившего процесс)

- `-l` -- отображает подробную информацию о процессах, включая такие поля, как F (флаги), S (состояние).

```
UID   PID  PPID        F CPU PRI NI       SZ    RSS WCHAN     S             ADDR TTY           TIME CMD
501 17384 17377     4006   0  31  0 33663028      8 -      Ss                  0 ttys001    0:00.14 /bin/zsh -il
```

- `-j` -- отображает информацию о процессах в формате, который включает в себя информацию о группах процессов (job control).

```
USER    PID  PPID  PGID   SESS JOBC STAT   TT       TIME COMMAND
tsyar 17384 17377 17384      0    0 Ss   s001    0:00.14 /bin/zsh -il
```
<a name="state"></a>
#### Состояния

* `R` (running or runnable) -- __выполняется / может выполнятся__; система не различает -- выполняется процесс или стоит в очереди готовых на выполнение процессов; планировщику нужно поставить процесс в очередь, а диспетчер выделяет процессу квант процессорного времени.
* `S` (interruptble sleep) -- __прерываемый сон__; ожидание прерывающего события.
* `D` (uninterruptible sleep (usually IO)) -- __непрерываемый сон__; например, блокировку процесса в ожидании ввода/вывода прервать нельзя.=
* `T ` (terminate) -- __остановленный процесс__.
* `Z ` (zombie) -- статус процесса -- зомби, у кого отобраны все ресурсы, кроме последнего -- дескриптора (выделяется процессу при его создании), который содержит поле _state_, указатель на процесс -- предок, указатель на родственные процессы.

<a name="flags"></a>
#### Флаги

* `0` -- был системный вызов `fork()`, был системный вызов `exec()` (переводит процесс на новое адресное пространство, что делается для запуска на выполнение новой программы, которая передается в качестве параметра).

* `1` -- был системный вызов `fork()` (создает новый процесс -- потомок), но системного вызова `exec()` не было.

* `4` -- показывает, что процесс выполняется с правами __супер пользователя__.

__Супер пользователь__ -- процесс, который имеет доступ к функциям ядра.

<a name="lab1_2"><h2> Задание 2</h2></a>

Напишите программу, в которой создается дочерний процесс и организуйте как в предке, так и в потомке бесконечные циклы, в которых выводятся идентификаторы процессов с помощью системного вызова `getpid()`.

```
#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>

int main(void)
{
    int childpid;
    if ((childpid = fork()) == -1) {
        perror("Can't fork\n"); 
        exit(1);
    } else if (childpid == 0) {
        while(1) printf(" %d", getpid());
    } else {
        while(1) printf(" %d",getpid());
    }
    exit(0);
}
```
В случае успеха в родительском процессе возвращаемое значение -- PID дочернего процесса, а в дочернем -- 0.  В случае неудачи -1 -- в родительском процессе, дочерний процесс не создается.

1. Запустите программу и посмотрите идентификаторы созданных процессов: предка и потомка.

2. Получите процесс -- зомби (с помощью команд `ps` и `kill`).

3. Получите осиротевший процесс (с помощью команд `ps` и `kill`).

Откроем два терминала: в одном -- запустим программу и получим вывод; в другой введем команду `ps -al`.

Видим два процесса, запущенные командой `.\app`, с PID 39471 (родительский процесс) и 39472 (дочерний процесс).

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/1.png)

Чтобы получить процесс -- зомби, нам необходимо __завершить дочерний процесс__ с помощью команды `kill 39472`. Смотрим на State и видим Z. 

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/2.png)

Теперь снова запустим нашу программу и выведем информацию о процессах.

Снова видим два процесса, запущенные командой `.\app`. В этот раз PID 39494 (родительский процесс) и 39495 (дочерний процесс).

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/3.png)

Чтобы получить осиротевший процесс, нам необходимо __завершить родительский процесс__ с помощью команды `kill 39494`. Смотрим на PPID (PID родительского процесса) и видим, что процесс усыновлен процессом PID 1 -- это процесс, который открыл терминал - __терминальный процесс__. 

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/4.png)

__ВАЖНО!__ Не всегда осиротевший процесс усыновляется терминальным процессом, иногда это может быть процесс - посредник. (Нужно будет (после получения осиротевшего процесса) вывести все процессы, найти там процесс - посредник, и посмотреть на его PPID -- ID терминального процесса.)

<a name="file_UNIX"></a>
#### Файловая система UNIX

Иерархическая ("деревянная"), состоит из файлов и каталогов. На каждом разделе диска создается собственная независимая файловая система.

##### Внутренняя структура файловой системы Unix

Каждый каталог и файл файловой системы имеет уникальное __полное имя - имя, задающее полный путь__.

В каждом каталоге содержатся два специальных имени, имя "." – определяющее текущий каталог, и имя "..", именующее "родительский" каталог данного каталога, т.е. каталог, непосредственно предшествующий данному в иерархии каталогов.

Linux поддерживает __inode__ (index node); виртуальная файловая система построена на четырех основных структурах: `struct superblock`, `struct inode`, `struct dentry`, `struct file`.

###### INODE

Каждый файл имеет свой __индексный дескриптор__, идентифицируемый по уникальному номеру в файловой системе, в которой располагается сам файл.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/6.png)

- Номер индексного дескриптора заносится в таблицу индексных дескрипторов в определённом месте устройства; по номеру индексного дескриптора ядро системы может считать содержимое inode, включая указатели данных и прочий контекст файла.

- Номер индексного дескриптора файла можно посмотреть, используя команду `ls -i`, а команда `ls -l` покажет информацию, хранящуюся в индексном дескрипторе.

__ВАЖНО!__ Имя файла в Unix/Linux не является его идентификатором, так как файл может иметь много имен, которые называются жесткими ссылками (hard links).

<a name="links"></a>
##### Жесткие и мягкие ссылки

 - __Жесткая ссылка__ -- еще одно имя файла. Файловая система отслеживает количество жестких ссылок на файл.

- __Мягкая ссылка, символическая ссылка__ -- это файл, который содержит полное имя другого файла.

Все жесткие ссылки на файл находятся в одной файловой системе. Симлинки на файл могут жить в разных файловых системах.

<a name="lab1_3"><h2> Задание 3</h2></a>

Создадим текстовый файл `file.txt` и создадим для него жесткую ссылку с помощью команды `ln <file_name> <hardlink_name>`.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/5.png)

Видим, что `inode` и содержимое файлов совпадают, а количество жестких ссылок на файл - 2. По сути, и `file.txt`, и `hardlink` - жесткие ссылки.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/7.png)

Создадим символьную ссылку для файла `file.txt` с помощью команды `ln -sf <file_name> <symlink_name>`.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/8.png)

Видим, что `inode` не совпадают, а тип файла - `l`. При открытии файла `symlink` откроется файл `file.txt`, так как `symlink` хранит в себе полной путь файла `file.txt`.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/9.png)

<a name="pipe"></a>
##### Программный канал (pipe)

Это буфер типа FIFO, в который можно «писать» информацию и из которого эту информацию можно «читать». Потоковая модель передачи данных - труба.

Программные каналы двух типов: _именованные_ и _неименованные_.

__Неименованные программные каналы__ могут использоваться для передачи сообщений __только между процессами__ - родственниками, т.е. между процессами, которые имеют общего предка.

<a name="lab1_4"><h2> Задание 4</h2></a>

 Создадим именнованный канал с именем `pipe` с помощью команды `mkfifo`. Видим, что тип файла - `p`.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/10.png)

Откроем второй терминал и попробуем записать/прочитать сообщение.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/11.png)

#### Конвейеры 

Конвейеры создаются с помощью неименованных программных каналов.

С помощью конвейеров удается комбинировать результаты выполнения разных команд, получая в результате новое качество.

Например, команда `ls` выводит информацию о файлах, а команда `wc` подсчитывает число строк в файле. Объединяя в конвейер эти команды, получаем возможность подсчитать количество файлов в каталоге.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/12.png)

#### Приоритеты процессов
   
В фазе “пользователь” приоритет процесса имеет 2 составляющие: __пользовательскую__ и __системную__. Значения этих составляющих задают поля дескриптора процесса `p_nice` и `p_cpu`.

Начальное значение пользовательской составляющей равно константе `NZERO` (=20). Пользовательская составляющая может быть изменена системным вызовом `nice` с аргументом, определяющим величину изменения поля `p_nice` в пределах от 0 до `NZERO` для непривилегированного процесса и от `–NZERO` до `+NZERO` для привилегированного.

Начальное значение системной составляющей в фазе пользователь равно 0. Ее изменение зависит от времени использования процессора. Для формирования системной составляющей приоритета используются прерывания от аппаратного таймера. Каждое прерывание по таймеру увеличивает значение поля `p_cpu` на 1.

Результирующий приоритет процесса в фазе «пользаватель» определяется по формуле: 

```
p_pri = (p_nice – NZERO) + (p_cpu / 16) + P_USER
```

`P_USER` - константа, по умолчанию равная 50.

Снижение пpиоpитета приводит к созданию фоновых процессов.

<a name="lab2"><h1> Лабораторная работа №2 </h1></a>

<a name="proc"></a>
__Процесс в Linux (как и в UNIX)__ -- программа, которая выполняется в отдельном защищенном виртуальном адресном пространстве.

Структура `proc` -- содержит всю информацию о процессе.

__Любая программа может начать выполняться только если другой процесс ее запустит__.

Часть времени процесс выполняется __в режиме задачи__ (пользовательском режиме) и тогда он __выполняет собственный код__, часть времени процесс выполняется __в режиме ядра__ и тогда он __выполняет реентерабельный код операционной системы__.

Ядро предоставляет системные вызовы для создания новых процессов и для управления запущенными процессами.

__Linux поддерживает параллельное выполнение процессов пользователя. Каждый процесс выполняется в собственном защищенном виртуальном адресном пространстве -- ни один процесс не обратиться в адресное пространство другого процесса__. Процессы защищены друг от друга и крах одного процесса никак не повлияет на другие выполняющиеся процессы и на всю систему в целом.

<a name="fork"></a>
#### Системный вызов `fork()`

Cоздает новый процесс, который является копией процесса - предка: процесс-потомок наследует адресное пространство процесса - предка, дескрипторы всех открытых файлов и сигнальную маску.

__В старых Unix-системах код предка копировался в адресное пространство процесса - потомка. В современных системах применяется, так называемая, оптимизация fork(): для процесса потомка создаются собственные таблицы страниц, но они ссылаются на адресное пространство процесса-предка. При этом для страниц адресного пространства предка права доступа меняются на _only read_ и устанавливается флаг - `copy-on-write`.__
__Если или предок, или потомок попытаются изменить страницу, возникнет исключение по правам доступа. Выполняя это исключение супервизор обнаружит флаг `copy-on-write` и создаст копию страницы в адресном пространстве того процесса, который пытался ее изменить.__ Таким образом код процесса - предка не копируется полностью, а создаются только копии страниц, которые редактируются.

__Как работает fork():__
1. Резервируется пространство свопинга для данных и стека процесса-потомка. 
2. Назначается идентификатор процесса PID и структура proc потомка.
3. Инициализируется структура proc потомка. Некоторые поля этой структуры копируются от процесса-родителя: идентификаторы пользователя и группы, маски сигналов и группа процессов. Часть полей инициализируется 0. Часть полей инициализируется специфическими для потомка значениями: PID потомка и его родителя, указатель на структуру proc родителя.
4. Создаются карты трансляции адресов для процесса-потомка.
5. Выделяется область u потомка и в нее копируется область u процесса-предка.
6. Изменяются ссылки области u на новые карты адресации и пространство свопинга.
7. Потомок добавляется в набор процессов, которые разделяют область кода программы, выполняемой процессом-родителем.
8. Постранично дублируются области данных и стека родителя и модифицируются карты адресации потомка.
9. Потомок получает ссылки на разделяемые ресурсы, которые он наследует: открытые файлы (потомок наследует дескрипторы) и текущий рабочий каталог.
10. Инициализируется аппаратный контекст потомка путем копирования регистров родителя.
11. Поместить процесс-потомок в очередь готовых процессов.
12. Возвращается PID в точку возврата из системного вызова в родительском процессе и 0 - в процессе-потомке.

<a name="lab2_1"><h2> Задание 1</h2></a>

Написать программу, запускающую не менее двух новых процессов системным вызовом fork():

1. В предке вывести собственный идентификатор (функция getpid()), идентификатор группы (функция getpgrp()) и идентификаторы потомков. 
2. В процессе - потомке вывести собственный идентификатор, идентификатор предка (функция getppid()) и идентификатор группы. 
3. Убедиться, что (при завершении процесса - предка) потомок, который продолжает выполняться, получает идентификатор предка (PPID), равный 1 или идентификатор процесса - посредника.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/13.png)

<a name="wait"></a>
#### Cистемный вызов `wait()`

Блокирует родительский процесс до момента завершения дочернего. При этом __процесс - предок получает статус завершения процесса - потомка__. Сведения о состоянии позволяют родительскому процессу определить статус завершения дочернего процесса, т.е. значение, возвращенное из функции `main` потомка или переданное функции `exit()`. Если `stat_loc` не равен пустому указателю, информация о состоянии будет записана в то место, на которое указывает этот параметр.

<a name="lab2_2"><h2> Задание 2</h2></a>

1. Написать программу по схеме первого задания, но в процессе - предке выполнить системный вызов wait(). 
2. Убедиться, что в этом случае идентификатор процесса потомка на 1 больше идентификатора процесса - предка.

Используется бесконечный цикл или `pause()`, т.e. нужно будет завершить дочерний процесс, который находится в прерываемом сне, через терминал.

![PAUSE](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/14.png)

<a name="exec"></a>
#### Cистемный вызов `exec()`

Cоздает низкоуровневый процесс: создаются таблицы страниц для адресного пространства программы, указанной в exec(), но программа на выполнение не запускается, так как это не полноценный процесс, имеющий идентификатор и дескриптор. Дрегими словами, cоздает таблицу страниц для адресного пространства программы, переданной ему в качестве параметра, а затем заменяет старый адрес таблицы страниц на новый. В результате системного вызова exec() адресное пространство процесса будет заменено на адресное пространство новой программы, а сам процесс будет возвращен в режим задачи с установкой указателя команд на первую выполняемую инструкцию этой программы.

__Так как новый процесс не создается, идентификатор процесса не меняется, но код, данные, куча и стек процесса заменяются кодом, данными, кучей и стеком новой программы.__

<a name="lab2_3"><h2> Задание 3</h2></a>

Написать программу, в которой процесс - потомок вызывает системный вызов `exec()`, а процесс - предок ждет завершения процесса - потомка. Следует создать не менее двух потомков.

![](https://github.com/unaun0/bmstu-os/blob/main/semester-01/lab/lab-processes/assets/16.png)

Видим два приглашения на ввод. Это связано с тем, что процесс блокирован в ожидании завершения операции ввода с клавиатуры. То есть, два процесса стоят в очереди готовых на выполнение процессов, один из них получает свой квант времени, выполняется, после чего блокируется в ожидании ввода, следовательно, теряет оставшуюся часть кванта. Операционная система переключается на выполнение следующего процесса в очереди, получившего квант.

__Операционная система управляет переключением между процессами. Каждый процесс получает свой квант процессорного времени на выполнение. Если процесс блокируется до того, как его квант времени закончился, оставшаяся часть кванта теряется. Операционная система переключается на другой процесс (это называется вытеснением).__

<a name="signal"></a>
<a name="lab2_4"><h2> Задание 4</h2></a>

Написать программу, в которой предок и потомок обмениваются сообщением через неименованный программный канал (системный вызов `pipe()`).

**Сигнал** -- способ информирования процесса ядром о происшествии какого-то события. Если возникает несколько однотипных событий, процессу будет подан только один сигнал. Сигнал означает, что произошло событие, но ядро не сообщает сколько таких событий произошло.
Обычно, получение процессом сигнала предписывает ему завершиться. Вместе с тем процесс может установить собственную реакцию на получаемый сигнал. Например, получив сигнал процесс может его проигнорировать, или вызвать на выполнение некоторую программу, а после ее завершения продолжить выполнение с точки получения сигнала.

Неименованный программный канал не имеет имени, но имеет дескриптор, т.е. имеет `inode struct`. Неименованный программный канал могут использовать только родственные процессы.

- Для чего нужны сигналы в наших программах? С их помощью можно изменять ход выполнения процесса.
- Сигнал это какое событие относительно процесса? Асинхронное.

<a name="lab2_5"><h2> Задание 5</h2></a>

1. В программу с программным каналом включить собственный обработчик сигнала. 
2. Использовать сигнал для изменения хода выполнения программы.

<a name="daemon"></a>
**Демоны** — процессы, не имеющие управляющего терминала. Демоны ядра запускаются без управляющего терминала. Пользовательские демоны не имеют управляющего терминала из-за функции `setid`. 
Все демоны пользовательского уровня являются лидерами групп и сессий и единственными процессами в своих группах процессов и сессиях.


<a name="lab2_6"><h2> Задание 6</h2></a>
(См. пример Стивенс Раго стр. 504-510)

##### daemonize превращает программу в демон

- Функция `umask` используется для сброса маски режима создания файлов в значение 0. Маска режима создания файлов, наследуемая от запускающего процесса, может маскировать некоторые биты прав доступа.

- Вызывается `fork`, после чего родительский процесс завершается -- это делается для того, чтобы гарантировать, что дочерний процесс не будет лидером группы (необходимое условие для вызова функции `setsid`), чтобы командная оболочка думала, что команда выполнена, если демон был запущен как обычная команда оболочки.

- Функция `setsid` создает новую сессию, а процесс становится лидером новой сессии, лидером новой группы процесссов и лишается управляющего терминала.

- Закрывает все ненужные файловые дескрипторы для предотвращения удержания в открытом состоянии некоторых дескрипторов, унаследованных от родительского процесса.

- Открывает файл /dev/null, чтобы использовать его в качестве стандартного ввода/вывода и ошибок.

##### lockfile устанавливает блокировку на файл для предотвращения запуска нескольких экземпляров демона

##### already_running проверяет, запущен ли уже демон

- Cоздает или открывает файл /var/run/daemon.pid. 

- Устанавливает блокировку на файл с помощью fcntl().

- Если процесс уже запущен, программа завершает работу.

##### thr_fn - функция потока, которая ожидает сигналы и обрабатывает их

- Обрабатывает сигнал SIGHUP (обычно означает перезагрузку конфигурации) и SIGTERM (означает завершение работы).

#### main

- Запускает процесс демонзации.

- Проверяет, запущен ли уже демон.

- Создает поток для обработки сигналов.

- Периодически записывает в системный журнал текущее время через каждые несколько секунд.

Этот код создает демон, который записывает в системный журнал текущее время. Он также обрабатывает системные сигналы, такие как SIGHUP (обновление конфигурации) и SIGTERM (завершение работы демона).

Для меня: 

```cat /var/run/mydaemon.pid```

```ps -ajx```

```kill [-HUP][-1] <PID>```

Этот сигнал обычно используется для перезагрузки конфигурации демона без остановки процесса.

```kill [-TERM][-15] <PID>```

Это сигнал для корректного завершения работы процесса. В отличие от SIGKILL (сигнал 9), который немедленно убивает процесс без возможности его завершить корректно, SIGTERM дает процессу время на обработку завершения.

```log show --predicate 'process == "mydaemon"' --info```

#### Основной поток

После запуска демона основной поток переходит в бесконечный цикл и выполняет основную работу — в данном случае он каждую секунду записывает текущее время в системный журнал (syslog). этот поток не занимается обработкой сигналов, чтобы не блокировать выполнение основной задачи.

#### Второй поток (обработка сигналов)

Этот поток создается с помощью функции pthread_create и выполняет функцию thr_fn, в которой обрабатываются сигналы с использованием функции sigwait(). Он предназначен для того, чтобы обработка сигналов не прерывала выполнение основной задачи (запись времени в журнал).
Второй поток блокирует выполнение, ожидая поступления одного из сигналов, таких как SIGHUP или SIGTERM. Если бы основной поток ожидал сигналы (использовал sigwait), он бы блокировался и не мог выполнять свою основную задачу — вывод текущего времени.

#### Демоны MacOS

**Процессы ядра** - это особые процессы, они существуют все время, пока работает система. Эти процессы обладают привилегиями суперпользователя и не имеют ни управляющего терминала, ни строки команды.

- launchd

Управление демонами и службами.

- kerneld 

Взаимодействие с аппаратными ресурсами, памятью, процессами и модулями ядра.

- opendirectoryd

Управление учетными записями и аутентификацией.

- configd

Отслеживание изменений конфигурации сети и уведомление других служб о соответствующих изменениях.

- diskarbitrationd

Отвечает за монтирование/размонтирование дисков и работу с файловыми системами.

- securityd

Управляет доступом к ключам, сертификатам и безопасным контейнерам (например, Keychain).

- WindowServer

Это демон графической подсистемы macOS, который управляет окнами приложений и их отображением на экране.

- distnoted

Управляет отправкой уведомлений по всей системе, которые могут обрабатываться приложениями. Обрабатывает межпроцессные уведомления.

- coreaudiod
Управляет звуковой системой macOS, отвечает за воспроизведение, запись и обработку аудиоданных.

- locationd
Управляет геолокационными службами, предоставляя информацию о местоположении.

- logd
Отвечает за сбор и хранение системных логов.


#### Вопросы

Прокомментируйте информацию о вашем демоне в системе: почему 3 одинаковых ID и почему PPID = 1?

(Получить PID демона из LOCKFILE, вывести все процессы с помощью команды `ps -ajx`, найти там демона по PID из LOCKFILE)

- Демоны - процессы, не имеющие управляющего терминала. Демоны ядра запускаются без управляющего терминала. Пользовательские демоны не имеют управляющего терминала из-за функции `setid`. Все демоны пользовательского уровня являются лидерами групп и сессий и единственными процессами в своих группах процессов и сессиях. На MacOS у всех системных служб и демонов PPID = 1, так как процесс launchd управляет всеми демонами и службами.

Как реализован в ядре системный вызов open?

- Системный вызов open предназначен для выполнения операции открытия файла и, в случае ее удачного осуществления, возвращает файловый дескриптор открытого файла (неотрицательное целое число).

Почему у демона не должно быть управляющего терминала?

- Демон — это процесс, который выполняется в фоновом режиме и обычно находится вне контроля пользователя.

Почему демон должен существовать в единственном экземпляре?

- Чтобы одна работа не выполнялась несколько раз, не возникало ошибок.

Почему файл блокировки требует у демона прав суперпользователя?

- Вывести права доступа директории `ls -la`, показать, что право на запись только у суперпользователя (root).

Показать точку, с какого момента процесс становится демоном.

- Показать в main() вызов функции daemonize.

Сколько потоков? Что делает дополнительный поток?

- Два потока: основной и дополнительный. Основной выводит время в системный журнал, дополнительный - ожидает сигнал.
