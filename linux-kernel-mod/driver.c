#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>

MODULE_AUTHOR("group#3 members");
MODULE_DESCRIPTION("this module print group#3's members name");
MODULE_LICENSE("GPL");

static int __init printing_members(void) {
    printk(KERN_INFO "Pouya Sadeghi, Ali Ataollahi, Ali Hodaee");
    return 0;
}
static void __exit exiting_module(void) {
    printk(KERN_INFO "we are group#3");
}

module_init(printing_members);
module_exit(exiting_module);