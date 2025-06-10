#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/time.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tskhovrebova Yana");
MODULE_ALIAS("myvfs");
MODULE_VERSION("1.0");

#define MAGIC_NUMBER 0xB16B00B5
#define SLAB_NAME "myvfs_cache"

struct myvfs_inode {
    int i_mode;             // режим доступа
    unsigned long i_ino;    // номер inode, уникальный идентификатор в рамках ФС
};

static void myvfs_put_super(struct super_block *sb) {
    printk(KERN_INFO "+ myvfs: myfs_put_super\n");
}

static int myvfs_statfs(struct dentry *dentry, struct kstatfs *buf) {
    printk(KERN_INFO "+ myvfs: call myvfs_statfs\n");
    return simple_statfs(dentry, buf);
}

int myvfs_delete_inode(struct inode *inode) {
    printk(KERN_INFO "+ myvfs: call myvfs_delete_inode\n");
    return generic_delete_inode(inode);
}

// Таблица указателей на функции, которые ядро вызывает при работе с super_block файловой системы
static struct super_operations const myvfs_super_operations = {
    .put_super = myvfs_put_super,       // Вызывается ядром при размонтировании файловой системы
    .statfs = myvfs_statfs,             // Вызывается при запросе статистики о файловой системе
    .drop_inode = myvfs_delete_inode,   // Вызывается ядром, когда inode "больше не нужен"
};

// Функция вызывается при монтировании файловой системы. 
// Она инициализирует super_block, создаёт корневой inode и dentry.
static int myvfs_fill_super(struct super_block *sb, void *data, int silent) {
    printk(KERN_INFO "+ myvfs: call myvfs_fill_super\n");
    // Инициализация super_block
                                        // Устанавливаем размер блока
    sb->s_blocksize = PAGE_SIZE;        // Размер страницы памяти (обычно 4096 байт)
    sb->s_blocksize_bits = PAGE_SHIFT;  // log₂(PAGE_SIZE), используется для битового сдвига при расчётах

    sb->s_magic = MAGIC_NUMBER;         // Уникальный магический номер ФС. Используется системой для распознавания типа ФС. 
    sb->s_op = &myvfs_super_operations; // Назначаем таблицу суперопераций
    sb->s_time_gran = 1;                // Устанавливает точность временных меток, в наносекундах. 1 означает максимальную точность.

    // Создание корневого inode
    // Выделяет и инициализирует новый inode, привязанный к суперблоку. Это корневая директория /.
    struct inode *inode = new_inode(sb);
    if (!inode) {
        printk(KERN_ERR "+ myvfs: ERR: new_inode\n");
        return -ENOMEM;
    }
                                                        // Установка временных меток
    inode_set_atime_to_ts(inode, current_time(inode));  // Время последнего доступа
    inode_set_mtime_to_ts(inode, current_time(inode));  // Время последней модификации
    inode_set_ctime_to_ts(inode, current_time(inode));  // Время последнего изменения inode
                                    // Настройка inode как директории
    inode->i_ino = 1;               // Номер inode (1 — корень)                   
    inode->i_mode = S_IFDIR | 0755; // S_IFDIR — это директория; 0755 — разрешения: rwxr-xr-x.
                                                // Назначаем стандартные операции для директорий
    inode->i_op = &simple_dir_inode_operations; // Обработка lookup и других inode-операций.
    inode->i_fop = &simple_dir_operations;      // Открытие, чтение и др. функций для директорий.
    set_nlink(inode, 2);    // Число жёстких ссылок: директория всегда имеет минимум 2 - на себя (.) 
                            // и на родительскую (в случае корня — тоже на себя).
    sb->s_root = d_make_root(inode); // Создаёт корневую точку монтирования (dentry) и связывает её с inode.
    if (!sb->s_root) {
        printk(KERN_ERR "+ myvfs: ERR: d_make_root\n");
        iput(inode);
        return -ENOMEM;
    }
    return 0;
}

// Эта функция вызывается ядром при попытке монтирования файловой системы.
static struct dentry *myvfs_mount(struct file_system_type *type, int flags, const char *dev, void *data) {
    // Аргументы:
    // type — указатель на структуру file_system_type, описывающую тип ФС (в нашем случае myvfs_type).
    // flags — флаги монтирования (например, MS_RDONLY, MS_NOEXEC и т.д.).
    // dev — устройство, с которым ассоциирована ФС (может быть NULL для nodev).
    // data — дополнительные данные, переданные при монтировании (например, опции монтирования).

    // Описание: Функция mount_nodev() используется для монтирования файловой системы, 
    //                                 которая не требует реального блочного устройства.
    // myvfs_fill_super — функция, которая инициализирует структуру super_block.
    // Возвращает корневой dentry (struct dentry *) или ERR_PTR при ошибке.
    struct dentry *const entry = mount_nodev(type, flags, data, myvfs_fill_super);
    if (IS_ERR(entry))
        printk(KERN_ERR "+ myvfs: mounting failed\n");
    else
        printk(KERN_INFO "+ myvfs: mounted\n");
    // Возвращает результат монтирования — dentry, представляющий корень смонтированной ФС.
    return entry;
}

// Функция размонтирования файловой системы
static void myvfs_kill_super(struct super_block *sb) {
    // sb — указатель на структуру super_block, представляющую монтированную ФС.
    printk(KERN_INFO "+ myvfs: call myvfs_kill_super\n");
    // Освобождает ресурсы суперблока для файловых систем, не имеющих устройств (анонимных). 
    // Используется, когда mount_nodev применялась для монтирования.
    kill_anon_super(sb);
}

// Cтруктура описывает тип файловой системы и используется для её регистрации в ядре.
static struct file_system_type myvfs_type = {
    .owner = THIS_MODULE,
    .name = "myvfs",        // Имя файловой системы. Именно его указывают при монтировании (mount -t myvfs none /mnt)
    .mount = myvfs_mount,   // Указатель на функцию монтирования файловой системы (myvfs_mount)
    .kill_sb = myvfs_kill_super, // Указатель на функцию размонтирования/уничтожения суперблока (myvfs_kill_super).
    .fs_flags = FS_USERNS_MOUNT, // Указывает, что файловая система может монтироваться внутри пользовательских пространств имен (user namespaces) 
};

static struct kmem_cache *cache = NULL; // Указатель на кэш объектов kmem_cache. 
                                        // Используется для эффективного выделения и освобождения памяти под объекты типа myvfs_inode.
static void **cache_mem = NULL; // Массив указателей на выделенную память из кэша (слэба). 
                                // Хранит адреса конкретных объектов, выделенных через kmem_cache_alloc.
static int cached_count = 256;  // Количество объектов myvfs_inode, которые мы хотим заранее выделить в кэше.
module_param(cached_count, int, 0); // Позволяет задавать параметр cached_count при загрузке модуля, например: insmod myvfs.ko cached_count=512.
                                    // int — тип параметра, 0 — права доступа (0 означает, что параметр нельзя менять через /sys/module/...).

// Функция инициализации объектов kmem_cache
void f_init(void *p) {
    // Функция инициализации для каждого объекта в кэше при его создании.
    // В данном случае просто записывает в первые 4 байта объекта его собственный адрес 
    // (просто пример инициализации, обычно используется для установки начальных значений).
    *(int *)p = (int)p;
}

// Функция инициализации модуля
static int __init myvfs_init(void) {
    // Регистрирует файловую систему в ядре с помощью структуры myvfs_type.
    int ret = register_filesystem(&myvfs_type);
    if (ret) {
        printk(KERN_ERR "+ myvfs: can't register_filesystem\n");
        return ret;
    }
    // Выделяет память под массив указателей cache_mem размером PAGE_SIZE * размер указателя 
    // (можно считать это буфером для cached_count объектов).
    // GFP_KERNEL — тип выделения памяти для процессов в ядре.
    if (NULL == (cache_mem = kmalloc(sizeof(struct myvfs_inode *) * PAGE_SIZE, GFP_KERNEL))) {
        printk(KERN_ERR "+ myvfs: can't kmalloc cache\n");
        return -ENOMEM;
    }
    // Создаёт кэш для объектов myvfs_inode:
    // SLAB_NAME — имя кэша
    // sizeof(struct myvfs_inode) — размер одного объекта
    // 0 — смещение (обычно 0)
    // SLAB_HWCACHE_ALIGN — флаг выравнивания для кэш-линий процессора (для повышения производительности)
    // f_init — функция инициализации объекта
    if (NULL == (cache = kmem_cache_create(SLAB_NAME, sizeof(struct myvfs_inode), 0, SLAB_HWCACHE_ALIGN, f_init))) {
        printk(KERN_ERR "+ myvfs: can't kmem_cache_create\n");
        kmem_cache_destroy(cache);
        kfree(cache_mem);
        return -ENOMEM;
    }
    // Цикл выделяет cached_count объектов из кэша и сохраняет указатели на них в cache_mem.
    // Если не удаётся выделить объект, освобождаем ранее выделенные, уничтожаем кэш и выходим с ошибкой.
    for (int i = 0; i < cached_count; i++) {
        if (NULL == (cache_mem[i] = kmem_cache_alloc(cache, GFP_KERNEL))) {
            printk(KERN_ERR "+ myvfs: can't kmem_cache_alloc\n");
            kmem_cache_free(cache, *cache_mem);
            kmem_cache_destroy(cache);
            kfree(cache_mem);
            return -ENOMEM;
        }
    }
    printk(KERN_INFO "+ myvfs: alloc %d objects into slab: %s\n", cached_count, SLAB_NAME);
    printk(KERN_INFO "+ myvfs: object size %ld bytes, full size %ld bytes\n", sizeof(struct myvfs_inode), sizeof(struct myvfs_inode *) * PAGE_SIZE);
    printk(KERN_INFO "+ myvfs: module loaded\n");
    return 0;
}

// Это функция выгрузки (деинициализации) модуля
static void __exit myvfs_exit(void) {
    // Попытка отменить регистрацию файловой системы myvfs из ядра.
    int ret = unregister_filesystem(&myvfs_type);
    if (ret)
        printk(KERN_ERR "+ myvfs: can't unregister_filesystem\n");
    // Проходим циклом по всем ранее выделенным объектам из кэша cache.
    // Для каждого объекта вызываем kmem_cache_free — освобождаем память, выделенную для объекта myvfs_inode.
    // Таким образом освобождается вся память, выделенная под объекты файловой системы, которая была заранее закэширована. 
    for (size_t i = 0; i < cached_count; i++)
        kmem_cache_free(cache, cache_mem[i]);
    // Уничтожаем сам кэш kmem_cache, который был создан через kmem_cache_create.
    // Это освобождает внутренние структуры ядра, связанные с кэшом объектов.
    kmem_cache_destroy(cache);
    // Освобождаем память массива указателей cache_mem, который хранил адреса всех выделенных объектов из кэша.
    kfree(cache_mem);
    printk(KERN_INFO "+ myvfs: module unloaded\n");
}

// Это макросы, которые говорят ядру Linux, какие функции нужно вызвать при загрузке и выгрузке модуля.
module_init(myvfs_init);
module_exit(myvfs_exit);
