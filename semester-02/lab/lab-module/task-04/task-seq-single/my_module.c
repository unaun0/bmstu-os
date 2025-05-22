#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tskhovrebova Yana Rolandovna");

#define FILENAME "my_file"
#define DIRNAME "my_dir"
#define SYMLINK "my_symlink"
#define FILEPATH DIRNAME "/" FILENAME

#define PID_ARRAY_SIZE 3

static struct proc_dir_entry *my_dir;
static struct proc_dir_entry *my_file;
static struct proc_dir_entry *my_link;

static pid_t pids[PID_ARRAY_SIZE ] = {-1, -1, -1};
static int pcounter = 0;

static int single_show(struct seq_file *m, void *v) {
    int i;
    struct task_struct *task;
    printk(KERN_INFO "*) show\n");
    printk(KERN_INFO "*) SEQ_BUF_SIZE: %zu bytes = %zu pages\n", m->size, m->size / PAGE_SIZE);
    printk(KERN_INFO "*) m = %p, v = %p\n", m, v);
    /*
    if (v) {
        int *val = (int *)v;
        printk(KERN_INFO "v: %d\n", *val);
    } else {
        printk(KERN_INFO "v = NULL\n");
    }
    KERNEL: Killed
    */
    if (pcounter == 0) {
        seq_puts(m, "*) No PIDs specified. Write up to 3 PIDs to the file first.\n");
        return 0;
    }
    for (i = 0; i < pcounter; i++) {
        pid_t pid = pids[i];
        task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
        if (task) {
            seq_printf(m,
                       "*) Process information for PID %d:\n"
                       "*) Policy:   %d\n"
                       "*) Priority: %d\n"
                       "*) Flags:    %#x\n"
                       "*) State:    %#x\n"
                       "*) stime:    %llu\n"
                       "*) utime:    %llu\n\n",
                       pid,
                       task->policy,
                       task->prio,
                       task->flags,
                       task->__state,
                       (unsigned long long) task->stime,
                       (unsigned long long) task->utime
            );
            put_task_struct(task);
        }
        else {
            seq_printf(m, "*) Process with PID %d not found.\n\n", pid);
        }
    }
    return 0;
}

static int my_open(struct inode *inode, struct file *file) {
    printk(KERN_INFO "*) open\n");
    return single_open(file, single_show, NULL);
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *ppos) {
    printk(KERN_INFO "*) read: ppos = %lld\n", *ppos);
    ssize_t ret = seq_read(file, buf, len, ppos);
    printk(KERN_INFO "*) read: returned %zd\n", ret);
    return ret;
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos) {
    char char_buf[32];
    int ret, pid;
    printk(KERN_INFO "*) write\n");
    if (len > sizeof(char_buf) - 1)
        return -EINVAL;
    if (copy_from_user(char_buf, buf, len))
        return -EFAULT;
    char_buf[len] = '\0';
    if (len > 0 && char_buf[len - 1] == '\n')
        char_buf[len - 1] = '\0';
    if (pcounter >= PID_ARRAY_SIZE)
        return -EINVAL;
    ret = kstrtoint(char_buf, 10, &pid);
    if (ret) return -EINVAL;
    pids[pcounter++] = pid;
    return len;
}

static int my_release(struct inode *inode, struct file *file) {
    printk(KERN_INFO "*) release\n");
    return single_release(inode, file);
}

static const struct proc_ops fops = {
    .proc_open = my_open,
    .proc_read = my_read,
    .proc_write = my_write,
    .proc_release = my_release,
};

static int __init md_init(void)
{
    printk(KERN_INFO "*) Module init\n");
    my_dir = proc_mkdir(DIRNAME, NULL);
    if (!my_dir)
        return -ENOMEM;

    my_file = proc_create(FILENAME, 0666, my_dir, &fops);
    if (!my_file) {
        remove_proc_entry(DIRNAME, NULL);
        return -ENOMEM;
    }
    my_link = proc_symlink(SYMLINK, NULL, FILEPATH);
    if (!my_link) {
        remove_proc_entry(FILENAME, my_dir);
        remove_proc_entry(DIRNAME, NULL);
        return -ENOMEM;
    }
    printk(KERN_INFO "*) Module loaded\n");
    return 0;
}

static void __exit md_exit(void) {
    if (my_link)
        remove_proc_entry(SYMLINK, NULL);
    if (my_file)
        remove_proc_entry(FILENAME, my_dir);
    if (my_dir)
        remove_proc_entry(DIRNAME, NULL);
    printk(KERN_INFO "*) Module unloaded\n");
}

module_init(md_init);
module_exit(md_exit);
