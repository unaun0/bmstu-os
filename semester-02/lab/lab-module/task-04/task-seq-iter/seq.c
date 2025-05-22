#include <linux/init.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/jiffies.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Tskhovrebova Yana");

#define FILENAME "seqFile"
#define DIRNAME "seqDir"
#define SYMLINK "seqLink"
#define FILEPATH DIRNAME "/" FILENAME

#define MAX_PIDS   3

static struct proc_dir_entry *seqDir;
static struct proc_dir_entry *seqFile;
static struct proc_dir_entry *seqLink;

static pid_t pids[MAX_PIDS] = {-1, -1, -1};
static int count_pids = 0;

static void *seq_start(struct seq_file *m, loff_t *pos)
{
    printk(KERN_INFO "start\n");
    return (*pos < count_pids) ? &pids[*pos] : NULL;
}

static void *seq_next(struct seq_file *m, void *v, loff_t *pos)
{
    printk(KERN_INFO "next\n");
    printk(KERN_INFO "m = %p, v = %p, pos = %lld\n", m, v, *pos);
    (*pos)++;
    return (*pos < count_pids) ? &pids[*pos] : NULL;
}

static void seq_stop(struct seq_file *m, void *v)
{
    printk(KERN_INFO "stop\n");
}

static int seq_show(struct seq_file *m, void *v)
{
    printk(KERN_INFO "show\n");
    pid_t pid = *(pid_t *) v;
    struct task_struct *task;

    if (pid < 0)
    {
        seq_puts(m, "Invalid PID\n\n");
        return 0;
    }

    task = get_pid_task(find_get_pid(pid), PIDTYPE_PID);
    if (!task)
    {
        seq_printf(m, "Process with PID %d not found.\n\n", pid);
        return 0;
    }

    seq_printf(m,
               "Process information for PID %d:\n"
               "  Policy:   %d\n"
               "  Priority: %d\n"
               "  Flags:    %#x\n"
               "  State:    %#lx\n"
               "  stime:    %llu\n"
               "  utime:    %llu\n\n",
               pid,
               task->policy,
               task->prio,
               task->flags,
               task->__state,
               (unsigned long long) task->stime,
               (unsigned long long) task->utime
    );

    put_task_struct(task);
    return 0;
}

static const struct seq_operations seq_ops = {
        .start = seq_start,
        .next  = seq_next,
        .stop  = seq_stop,
        .show  = seq_show,
};

static int my_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "open\n");
    return seq_open(file, &seq_ops);
}

static ssize_t my_read(struct file *file, char __user *buf, size_t len, loff_t *ppos)
{
    printk(KERN_INFO "read\n");
    return seq_read(file, buf, len, ppos);
}

static ssize_t my_write(struct file *file, const char __user *buf, size_t len, loff_t *ppos)
{
    printk(KERN_INFO "write\n");
    char kbuf[32];
    int ret, pid;

    if (len >= sizeof(kbuf))
        return -EINVAL;

    if (copy_from_user(kbuf, buf, len))
        return -EFAULT;

    kbuf[len] = '\0';
    if (len > 0 && kbuf[len - 1] == '\n')
        kbuf[len - 1] = '\0';

    if (count_pids >= MAX_PIDS)
        return -EINVAL;

    ret = kstrtoint(kbuf, 10, &pid);
    if (ret)
        return -EINVAL;

    pids[count_pids++] = pid;
    return len;
}

static int my_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "release\n");
    return seq_release(inode, file);
}

static const struct proc_ops fops = {
        .proc_open    = my_open,
        .proc_read    = my_read,
        .proc_write   = my_write,
        .proc_release = my_release,
};

static void free_resources(void)
{
    if (seqLink)
        remove_proc_entry(SYMLINK, NULL);
    if (seqFile)
        remove_proc_entry(FILENAME, seqDir);
    if (seqDir)
        remove_proc_entry(DIRNAME, NULL);
}

static int __init md_init(void)
{
    if ((seqDir = proc_mkdir(DIRNAME, NULL)) == NULL)
        return -ENOMEM;

    if ((seqFile = proc_create(FILENAME, 0666, seqDir, &fops)) == NULL)
    {
        free_resources();
        return -ENOMEM;
    }

    if ((seqLink = proc_symlink(SYMLINK, NULL, FILEPATH)) == NULL)
    {
        free_resources();
        return -ENOMEM;
    }

    printk(KERN_INFO "Module loaded\n");
    return 0;
}

static void __exit md_exit(void)
{
    free_resources();
    printk(KERN_INFO "Module unloaded\n");
}

module_init(md_init);
module_exit(md_exit);
