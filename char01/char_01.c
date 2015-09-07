/* Copyright 2015, Santiago F. Maudet
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* !\brief This is a basic example of a char device.
 *         Basics of LKM are described.
 *         This module has a initialized buffer in kernel memory that you
 *         can read from user space.
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

static dev_t device;        //This variable stores the minor and major mumber of the device
struct cdev device_cdev;    //Kernel structure for char devices.

static char mem_data[] = "Hello World\n";   //Static buffer with information.


// Read function //
ssize_t dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    printk(KERN_INFO "In chardrv read routine.\n");
    printk(KERN_INFO "Count field is %lu.\n", count);
    printk(KERN_INFO "Offset is %lu.\n", *f_pos);
    if (mem_data[*f_pos] == '\0') {
        printk(KERN_INFO "End of string, returning zero.\n");
        return 0;  // EOF indicator
    }
    copy_to_user(buf, &mem_data[*f_pos], 1);
   *f_pos += 1;
   return 1;  // returned a single character
}


struct file_operations dev_fops = {
    .owner = THIS_MODULE,
    .read = dev_read,
};

static int __init dev_init(void)
{
    printk(KERN_INFO "module chardrv being loaded.\n");

    alloc_chrdev_region(&device, 0, 1, "char_01");

    printk(KERN_INFO "%s\n", format_dev_t(mem_data, device));

    cdev_init(&device_cdev, &dev_fops);

    device_cdev.owner = THIS_MODULE;
    cdev_add(&device_cdev, device, 1);

    return 0;
}

static void __exit dev_exit(void)
{
    printk(KERN_INFO "Unloading Module CHAR_01\n");
    cdev_del(&device_cdev);
    unregister_chrdev_region(device, 1);
}

module_init(dev_init);
module_exit(dev_exit);

MODULE_AUTHOR("Santiago F. Maudet, DPLab @ UTN.BA http://www.electron.frba.utn.edu.ar/dplab");
MODULE_LICENSE("GPL");
