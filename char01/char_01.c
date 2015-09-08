/* Copyright (C) 2015, Santiago F. Maudet
 * This file is part of char01 module.
 *
 * char01 module is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * char01 module is distributed in the hope that it will be useful,
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
 *         Usage:
 *         1) COMPILE = make
 *         2) Insert module into kernel (root): insmod char_01.ko
 *         3) Create /dev/char_01 node: mknod /dev/char_01 c [MAYOR_NUMBER] [MINOR_NUMBER]
 *         4) cat /dev/char_01 or use open and read from a C program.
 *
*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

#include <linux/types.h>
#include <linux/kdev_t.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>

#define CHAR_01_MINOR 0     //Start MINOR..
#define CHAR_01_N_DEVS 1    //Number o devices to register.
#define NUM_BYTES_TO_COPY 1 //Number of bytes to copy in copy_to_user call.


static dev_t device;        //This variable stores the minor and major mumber of the device
struct cdev device_cdev;    //Kernel structure for char devices.

static char mem_data[] = "Hello World\n";   //Static buffer with information.
static int result;  // Aux Variable.
static int nbytes;

// Read function //
ssize_t dev_read(struct file *filp, char __user *buf, size_t count, loff_t *f_pos) {
    /////Some info printed in /var/log/messages ///////
    printk(KERN_INFO "Entering dev_read function\n");
    printk(KERN_INFO "Count Parameter from user space %lu.\n", count);
    printk(KERN_INFO "Offset in buffer %lu.\n",(long unsigned int)*f_pos);
    ///////////////////////////////////////////////////

    if (mem_data[*f_pos] == '\0') { // f_pos tracks the position of the buffer.
        printk(KERN_INFO "Reachead end of buffer.\n");
        return 0;  // EOF indicator. All info was transmited.
    }

    nbytes = copy_to_user(buf, &mem_data[*f_pos], NUM_BYTES_TO_COPY); // Copy data from kernel to user.
    printk(KERN_INFO " %d bytes copied from kernel to user\n",nbytes);
    *f_pos += 1;  //Increase pointer.
    return 1;  // returned a single character. Ok
}

struct file_operations dev_fops = { //Struct File Operations, this module only supports read...
    .owner = THIS_MODULE,           // Tells who is owner of struct file_operations
    .read = dev_read,               // Function pointer init with dev_read function.
};



// Init Function //
static int __init dev_init(void)
{
    printk(KERN_INFO "Loading module char_01\n"); //Kernel Info

    // Dynamic Allocation of MAJOR Number for char_01 Module
    result = alloc_chrdev_region(&device,CHAR_01_MINOR,CHAR_01_N_DEVS, "char_01");

    //Can't get MAJOR Number, return...
    if(result < 0){
      printk(KERN_WARNING "char_01: can't get major\n");
      return result;
    }

    // Initialize struct cdev "device_cdev" with struct file_operations "dev_fops"
    cdev_init(&device_cdev, &dev_fops);

    device_cdev.owner = THIS_MODULE;  //Tells who is owner of struct cdev

    // Add device to kernel.
    result = cdev_add(&device_cdev, device,CHAR_01_N_DEVS);
    if(result < 0){
      printk(KERN_WARNING "char_01 can't be registered in kenel\n");
      return result;
    }
    printk(KERN_INFO "Correct Registration of device char_01...\n");
    return 0;
}

// Exit Function //
static void __exit dev_exit(void)
{
    printk(KERN_INFO "Unloading Module CHAR_01\n");
    cdev_del(&device_cdev); //Remove device form kernel.
    unregister_chrdev_region(device,CHAR_01_N_DEVS);//Release MAJOR and MINIOR Numbers.
}

module_init(dev_init); //Init Macro loaded with init function.
module_exit(dev_exit); //Exit Macro loaded with exit function.

MODULE_AUTHOR("Santiago F. Maudet, DPLab @ UTN.BA http://www.electron.frba.utn.edu.ar/dplab");
MODULE_LICENSE("GPL");


//Print
//printk(KERN_INFO "%s\n", format_dev_t(mem_data, device));
