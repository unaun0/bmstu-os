#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/fs_struct.h>
#include <linux/path.h>
#include <linux/time.h>
#include <linux/timex.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Yana Tskhovrebova");

static int __init my_module_init(void) {
    struct task_struct *task =  = &init_task;
    struct task_struct *kthreadd_task = NULL;

    printk(KERN_INFO "CUR - PID: %d, COMM: %s, PPID: %d, PCOMM: %s, STATE: %ld, ON_CPU: %d, PRIO: %d, S_PRIO: %d, N_PRIO: %d, POLICY: %ld, UTIME: %llu, STIME: %llu, SW_COUNT: %lu\n",
        current->pid, current->comm, current->parent->pid, current->parent->comm, 
        current->__state, task_cpu(current),  current->prio, current->static_prio, current->normal_prio, 
        current->policy, (unsigned long long)current->utime, (unsigned long long)current->stime, 
        current->nvcsw + current->nivcsw);

   
    do {
        printk(KERN_INFO "PID: %d, COMM: %s, PPID: %d, PCOMM: %s, STATE: %ld, ON_CPU: %d, PRIO: %d, S_PRIO: %d, N_PRIO: %d, POLICY: %ld, UTIME: %llu, STIME: %llu, SW_COUNT: %lu\n",
            task->pid, task->comm, task->parent->pid, task->parent->comm, 
            task->__state, task_cpu(task),  task->prio, task->static_prio, task->normal_prio, 
            task->policy, (unsigned long long)task->utime, (unsigned long long)task->stime, 
            task->nvcsw + task->nivcsw);
    } while ((task = next_task(task)) != &init_task);

    return 0;
}

static void __exit my_module_exit(void) {
    printk(KERN_INFO "Good by\n");
}

module_init(my_module_init);
module_exit(my_module_exit);