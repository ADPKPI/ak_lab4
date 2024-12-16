#include <linux/init.h>
#include <linux/module.h>
#include <linux/printk.h>
#include <linux/slab.h>
#include <linux/ktime.h>
#include <linux/list.h>

MODULE_AUTHOR("Prokhorenko Artem, IO-23");
MODULE_DESCRIPTION("Hello, world with parameters and time logging");
MODULE_LICENSE("Dual BSD/GPL");

// Параметр модуля
static uint count = 1;
module_param(count, uint, 0644);
MODULE_PARM_DESC(count, "Number of times to print 'Hello, world!'");

// Структура для збереження часу
struct time_list {
    struct list_head list;
    ktime_t time;
};

// Головний список
static LIST_HEAD(times);

static int __init hello_init(void)
{
    if (count == 0 || (count >= 5 && count <= 10)) {
        printk(KERN_WARNING "Warning: count is %u. Proceeding.\n", count);
    }

    if (count > 10) {
        printk(KERN_ERR "Error: count is too large (%u).\n", count);
        return -EINVAL;
    }

    for (uint i = 0; i < count; i++) {
        struct time_list *entry = kmalloc(sizeof(*entry), GFP_KERNEL);
        if (!entry) {
            printk(KERN_ERR "Error: Memory allocation failed.\n");
            return -ENOMEM;
        }
        entry->time = ktime_get();
        list_add_tail(&entry->list, &times);

        printk(KERN_INFO "Hello, world!\n");
    }

    return 0;
}

static void __exit hello_exit(void)
{
    struct time_list *entry, *tmp;

    list_for_each_entry_safe(entry, tmp, &times, list) {
        printk(KERN_INFO "Time: %llu ns\n", ktime_to_ns(entry->time));
        list_del(&entry->list);
        kfree(entry);
    }
}

module_init(hello_init);
module_exit(hello_exit);
