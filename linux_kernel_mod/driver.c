#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Group#3 members");
MODULE_DESCRIPTION("this module print group#3's members name");

static int __init print_members_name(void) {
    printk(KERN_INFO "Pouya Sadeghi, Ali Ataollahi, Ali Hodaee\n");
    return 0;
}

static void __exit outro(void) {
    printk(KERN_INFO "we are group#3\n");
}

module_init(print_members_name);
module_exit(outro);