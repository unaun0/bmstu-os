# Загружаемые модули ядра

## Ошибки компиляции 

### Первый случай 

Сделать `make` раскоментив в `md2`:

```
printk("+ string returned md1_local() : %s\n", md1_local());
```

и показать, что случилась ошибка компиляции: 

```
/home/parallels/Desktop/task-02/md2.c:15:52: error: implicit declaration of function ‘md1_local’ [-Werror=implicit-function-declaration]
   15 |     printk("+ string returned md1_local() : %s\n", md1_local()); // 1
      |                                                    ^~~~~~~~~
```

Эта ошибка говорит о том, что компилятор не нашел объявления функции `md1_local` до ее использования в коде модуля `md2.c`. Функция `md1_local` определена как `static` в модуле `md1`, что означает, что она доступна только внутри этого модуля и не может быть использована в других модулях напрямую.

### Второй случай

Сделать `make` раскоментив в `md2`:

```
printk("+ string returned md1_noexport() : %s\n", md1_noexport()); 

// md.h
extern char *md1_data;
extern char *md1_proc(void);
// extern char *md1_noexport(void);
```

и показать, что случилась ошибка компиляции:

```
make -C /lib/modules/6.8.0-57-generic/build M=/home/parallels/Desktop/task-02 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-57-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  You are using:           gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  CC [M]  /home/parallels/Desktop/task-02/md1.o
/home/parallels/Desktop/task-02/md1.c:15:14: warning: no previous prototype for ‘md1_noexport’ [-Wmissing-prototypes]
   15 | extern char* md1_noexport( void ) {
      |              ^~~~~~~~~~~~
/home/parallels/Desktop/task-02/md1.c:9:14: warning: ‘md1_local’ defined but not used [-Wunused-function]
    9 | static char* md1_local( void ) {
      |              ^~~~~~~~~
  CC [M]  /home/parallels/Desktop/task-02/md2.o
In file included from ./include/asm-generic/bug.h:22,
                 from ./arch/x86/include/asm/bug.h:87,
                 from ./include/linux/bug.h:5,
                 from ./arch/x86/include/asm/paravirt.h:19,
                 from ./arch/x86/include/asm/cpuid.h:62,
                 from ./arch/x86/include/asm/processor.h:19,
                 from ./arch/x86/include/asm/timex.h:5,
                 from ./include/linux/timex.h:67,
                 from ./include/linux/time32.h:13,
                 from ./include/linux/time.h:60,
                 from ./include/linux/stat.h:19,
                 from ./include/linux/module.h:13,
                 from /home/parallels/Desktop/task-02/md2.c:2:
/home/parallels/Desktop/task-02/md2.c: In function ‘md_init’:
/home/parallels/Desktop/task-02/md2.c:14:56: error: implicit declaration of function ‘md1_noexport’ [-Werror=implicit-function-declaration]
   14 |     printk( "+ string returned md1_noexport() : %s\n", md1_noexport() ); // 2
      |                                                        ^~~~~~~~~~~~
./include/linux/printk.h:429:33: note: in definition of macro ‘printk_index_wrap’
  429 |                 _p_func(_fmt, ##__VA_ARGS__);                           \
      |                                 ^~~~~~~~~~~
/home/parallels/Desktop/task-02/md2.c:14:5: note: in expansion of macro ‘printk’
   14 |     printk( "+ string returned md1_noexport() : %s\n", md1_noexport() ); // 2
      |     ^~~~~~
/home/parallels/Desktop/task-02/md2.c:14:13: warning: format ‘%s’ expects argument of type ‘char *’, but argument 2 has type ‘int’ [-Wformat=]
   14 |     printk( "+ string returned md1_noexport() : %s\n", md1_noexport() ); // 2
      |             ^~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  ~~~~~~~~~~~~~~
      |                                                        |
      |                                                        int
./include/linux/printk.h:429:25: note: in definition of macro ‘printk_index_wrap’
  429 |                 _p_func(_fmt, ##__VA_ARGS__);                           \
      |                         ^~~~
/home/parallels/Desktop/task-02/md2.c:14:5: note: in expansion of macro ‘printk’
   14 |     printk( "+ string returned md1_noexport() : %s\n", md1_noexport() ); // 2
      |     ^~~~~~
/home/parallels/Desktop/task-02/md2.c:14:50: note: format string is defined here
   14 |     printk( "+ string returned md1_noexport() : %s\n", md1_noexport() ); // 2
      |                                                 ~^
      |                                                  |
      |                                                  char *
      |                                                 %d
cc1: some warnings being treated as errors
make[3]: *** [scripts/Makefile.build:243: /home/parallels/Desktop/task-02/md2.o] Error 1
make[2]: *** [/usr/src/linux-headers-6.8.0-57-generic/Makefile:1925: /home/parallels/Desktop/task-02] Error 2
make[1]: *** [Makefile:240: __sub-make] Error 2
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-57-generic'
make: *** [Makefile:12: all] Error 2
```

Этот тип ошибки возникает, потому что компилятор не может найти объявление функции `md1_noexport` в момент компиляции модуля `md2.c`. Это происходит из-за отсутствия прототипа функции в заголовочном файле `md.h`.

### Третий случай

Сделать `make` раскоментив в `md2`:

```
printk("+ string returned md1_noexport() : %s\n", md1_noexport()); 

// md.h
extern char *md1_data;
extern char *md1_proc(void);
extern char *md1_noexport(void);
```

и показать, что случилась ошибка при сборке:

```
make -C /lib/modules/6.8.0-57-generic/build M=/home/parallels/Desktop/task-02 modules
make[1]: Entering directory '/usr/src/linux-headers-6.8.0-57-generic'
warning: the compiler differs from the one used to build the kernel
  The kernel was built by: x86_64-linux-gnu-gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  You are using:           gcc-12 (Ubuntu 12.3.0-1ubuntu1~22.04) 12.3.0
  CC [M]  /home/parallels/Desktop/task-02/md1.o
/home/parallels/Desktop/task-02/md1.c:9:14: warning: ‘md1_local’ defined but not used [-Wunused-function]
    9 | static char* md1_local( void ) {
      |              ^~~~~~~~~
  CC [M]  /home/parallels/Desktop/task-02/md2.o
  CC [M]  /home/parallels/Desktop/task-02/md3.o
  MODPOST /home/parallels/Desktop/task-02/Module.symvers
ERROR: modpost: "md1_noexport" [/home/parallels/Desktop/task-02/md2.ko] undefined!
make[3]: *** [scripts/Makefile.modpost:145: /home/parallels/Desktop/task-02/Module.symvers] Error 1
make[2]: *** [/usr/src/linux-headers-6.8.0-57-generic/Makefile:1877: modpost] Error 2
make[1]: *** [Makefile:240: __sub-make] Error 2
make[1]: Leaving directory '/usr/src/linux-headers-6.8.0-57-generic'
make: *** [Makefile:12: all] Error 2
```

Ошибка говорит о том, что при компиляции модуля `md2` не удается найти определение функции или символа `md1_noexport`. Ты определяешь функцию `md1_noexport`, но она не экспортируется через `EXPORT_SYMBOL()`. При попытке использовать `md1_noexport` в других модулях, система не может найти её, так как она не экспортирована, что вызывает ошибку `undefined symbol`.
Произошла ошибка при сборке, так как с точки зрения Си всё нормально, но с точки зрения ядра - не порядок.

## Ошибки после компиляции

Пытаемся сделать `sudo insmod md2.ko` показываем, что не возможно, так как первый модуль не загружен (нужен его физический адрес !!!).

```
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo insmod md2.ko
[sudo] password for parallels: 
insmod: ERROR: could not insert module md2.ko: Unknown symbol in module

parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo dmesg | tail -2
[ 2944.620502] md2: Unknown symbol md1_data (err -2)
[ 2944.620514] md2: Unknown symbol md1_proc (err -2)
```

Загружаем `md1.ko` загружаем `md2.ko`  - теперь всё ок. С помощью `sudo lsmod` показать связь:

```
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo insmod md1.ko
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo insmod md2.ko

parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ lsmod | grep md
Module                  Size  Used by
md2                    12288  0
md1                    12288  1 md2

parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo dmesg | tail -4
[ 2984.482506] + module md1 start!
[ 2987.689136] + module md2 start!
[ 2987.689141] + data string exported from md1 : Hello World!
[ 2987.689143] + string returned md1_proc() is : Hello World!
```

Пытаемся выгрузить модуль `md1`:

```
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo rmmod md1
rmmod: ERROR: Module md1 is in use by: md2
```

Ошибка происходит потому что наш модуль `md1` связан с `md2`. То есть сначала надо выгрузить `md2`.

(число в квадратных скобках в выводе `dmesg` — это время с момента загрузки системы (`boot time`) в секундах)

```
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo rmmod md2
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo rmmod md1
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo dmesg | tail -2
[ 3531.873609] + module md2 unloaded!
[ 3534.322976] + module md1 unloaded!
```

Загрузим 1й и 3й модуль ядра, возвращающий ошибку.

```
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo insmod md1.ko
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo dmesg | tail -1
[ 3876.352692] + module md1 start!

parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo insmod md3.ko
insmod: ERROR: could not insert module md3.ko: Operation not permitted
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo dmesg | tail -4
[ 3876.352692] + module md1 start!
[ 3917.256303] + module md3 start!
[ 3917.256308] + data string exported from md1 : Hello World!
[ 3917.256309] + string returned md1_proc() is : Hello World!
```

```
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ lsmod | grep md
Module                  Size  Used by
md1                    12288  0
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo rmmod md3
rmmod: ERROR: Module md3 is not currently loaded
parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo rmmod md1

parallels@ubuntu-linux-22-04-02-desktop:~/Desktop/task-02$ sudo dmesg | tail -1
[ 4229.442375] + module md1 unloaded!
```

В Linux при загрузке модуля с помощью `insmod` вызывается функция `__init` — у вас это `md_init`. Эта функция должна вернуть `0`, если инициализация прошла успешно, или отрицательное значение, если произошла ошибка.

```
static int __init md_init( void ) {
    printk( "+ module md3 start!\n" );
    printk( "+ data string exported from md1 : %s\n", md1_data );
    printk( "+ string returned md1_proc() is : %s\n", md1_proc() );
    return -1; // <-- это говорит ядру: "ошибка загрузки"
}
```

Что происходит:
1. Вызывается `insmod md3.ko`.
2. Функция `md_init()` возвращает -1.
3. Ядро сразу же отменяет загрузку модуля, даже несмотря на то, что printk успел отработать (поэтому логи вы видите в dmesg).
4. Модуль не отображается в `lsmod`, потому что он не загружен.
5. Попытка удалить его (`rmmod md3`) даёт ошибку: `"Module md3 is not currently loaded"`.

Подтверждение:

```
insmod: ERROR: could not insert module md3.ko: Operation not permitted
```

Это стандартное поведение ядра: отказ от загрузки модуля при ненулевом результате `__init`.