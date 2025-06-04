#include <linux/module.h> 
#include <linux/kernel.h> 
#include <linux/init.h>  
#include <linux/fs.h>
#include <linux/slab.h>
#include <linux/time.h>

// Лицензия модуля ядра
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tskhovrebova Yana");
MODULE_DESCRIPTION("VFS");

// Уникальный идентификатор файловой системы
#define A23FS_MAGIC 0xDEADBEEF
#define SLAB_NAME "a23fs_slab"

// Определение собственной структуры inode
struct myfs_inode {
    int i_mode;                      // Режим (тип и права доступа)
    unsigned long i_ino;            // Номер inode
} myfs_inode;

// Счётчик, используемый в конструкторе slab
static int sco = 0;
static int number = 31;             // Просто число (не используется в коде)

// Указатель на кеш slab-allocator'а
struct kmem_cache *cache = NULL; 
static void **line = NULL;         // Указатель на массив указателей для slab
static int size = sizeof(struct myfs_inode); // Размер объекта в кеше

// Конструктор slab-объекта
void co(void* p) { 
    *(int*)p = (int)p;              // Присваивает указателю значение как int
    sco++;                          // Увеличивает счётчик конструкций
}

// Создание inode в рамках суперблока
static struct inode *myfs_make_inode(struct super_block *sb, int mode) {
    struct inode *ret = new_inode(sb);  // Выделяет новый inode
    if (ret) {
        inode_init_owner(&init_user_ns, ret, NULL, mode); // Устанавливает владельца и права
        ret->i_size = PAGE_SIZE;      // Размер страницы
        ret->i_atime = ret->i_mtime = ret->i_ctime = current_time(ret); // Устанавливает временные метки
        ret->i_private = &myfs_inode; // Приватное поле, ссылается на нашу структуру
    }
    printk(KERN_INFO "+: a23fs - new inode was creared\n");
    return ret;
}

// Освобождение суперблока
static void myfs_put_super(struct super_block *sb) {
    printk(KERN_INFO "+: a23fs - super block was destroyed\n");
}

// Операции с суперблоком
static struct super_operations const myfs_super_ops = {
    .put_super = myfs_put_super,          // Функция уничтожения суперблока
    .statfs = simple_statfs,              // Используется стандартная функция statfs
    .drop_inode = generic_delete_inode,   // Удаление inode
};

// Инициализация суперблока при монтировании
static int myfs_fill_sb(struct super_block *sb, void *data, int silent) {
    struct inode *root = NULL;

    sb->s_blocksize = PAGE_SIZE;          // Размер блока
    sb->s_blocksize_bits = PAGE_SHIFT;   // Смещение для блока
    sb->s_op = &myfs_super_ops;          // Операции суперблока
    sb->s_magic = A23FS_MAGIC;           // Уникальный номер ФС

    root = myfs_make_inode(sb, S_IFDIR | 0755); // Создание корневого inode (каталог)

    if (!root) {
        printk(KERN_ERR "+: a23fs inode allocation failed\n");
        return -ENOMEM;
    }

    root->i_op = &simple_dir_inode_operations; // Операции над каталогами
    root->i_fop = &simple_dir_operations;      // Файловые операции для каталогов
    sb->s_root = d_make_root(root);            // Создание корневого dentry

    if (!sb->s_root) {
        printk(KERN_ERR "+: a23fs root creation failed\n");
        iput(root);
        return -ENOMEM;
    }

    printk(KERN_INFO "+: a23fs - root was creared\n");
    return 0;
}

// Монтирование файловой системы
static struct dentry *myfs_mount(struct file_system_type *type, int flags, char const *dev, void *data)
{
    struct dentry *const entry = mount_nodev(type, flags, data, myfs_fill_sb); // Без устройства

    if (IS_ERR(entry)) {
        printk(KERN_ERR "+: a23fs mounting failed\n");
    } else {
        printk(KERN_DEBUG "+: a23fs mounted\n");
    }

    return entry;
}

// Демонтирование ФС
static void myfs_kill_anon_super(struct super_block *sb)
{
    printk(KERN_INFO "+: a23fs - My kill_anon_super\n");
    return kill_anon_super(sb); // Освобождает анонимный суперблок
}

// Регистрация ФС
static struct file_system_type myfs_type = {
    .owner = THIS_MODULE,
    .name = "a23fs",
    .mount = myfs_mount,
    .kill_sb = myfs_kill_anon_super,
};

// Загрузка модуля ядра
static int __init myfs_init(void)
{
    line = kmalloc(sizeof(void*), GFP_KERNEL); // Выделение памяти под указатель
    if(!line) { 
        printk(KERN_ERR "+: kmalloc error\n"); 
        return -ENOMEM;
    }
 
	cache = kmem_cache_create(SLAB_NAME, size, 0, 0, co); // Создание кеша slab
    if(!cache) { 
        printk(KERN_ERR "+: kmem_cache_create error\n"); 
        kfree(line); 
	    return -ENOMEM;  
    } 

    if (!((*line) = kmem_cache_alloc(cache, GFP_KERNEL))) { // Выделение объекта из кеша
        printk(KERN_ERR "+: kmem_cache_alloc error\n"); 
        kmem_cache_free(cache, *line);
        kmem_cache_destroy(cache);
        kfree(line);
        return -ENOMEM;
    }

    int ret = register_filesystem(&myfs_type); // Регистрация файловой системы
    if (ret != 0) {
        printk(KERN_ERR "+: a23fs module cannot register filesystem\n");
        return ret;
    }

    printk(KERN_INFO "+: a23fs_module loaded\n");
    return 0;
}

// Выгрузка модуля ядра
static void __exit myfs_exit(void)
{
    kmem_cache_free(cache, *line);     // Освобождение объекта кеша
    kmem_cache_destroy(cache);         // Уничтожение кеша
    kfree(line);                       // Освобождение line

    int ret = unregister_filesystem(&myfs_type); // Отмена регистрации ФС
    if (ret != 0) {
        printk(KERN_ERR "+: a23fs_module cannot unregister filesystem\n");
    }

    printk(KERN_DEBUG "+: a23fs_module unloaded\n");
}

// Установка точек входа и выхода
module_init(myfs_init);
module_exit(myfs_exit);