#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/kobject.h>
#include <linux/i2c.h>
/*
Moses Mbugua
Lab 6.1
*/

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Quiic Accelerometer Sysfs Module");
MODULE_VERSION("0.1");

struct i2c_adapter* i2c_dev;
struct i2c_client* i2c_client;

enum { FOO_SIZE_MAX = 10 };
static int foo_size;
static char x_axis[FOO_SIZE_MAX];
static char y_axis[FOO_SIZE_MAX];
static char z_axis[FOO_SIZE_MAX];

static struct i2c_board_info __initdata board_info[] =  {
	{
		I2C_BOARD_INFO("MMA8452Q", 0x1d),
	}
};

//This is a function that will handle the file read request
static ssize_t x_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buff)
{
    //this will just copy the internal kernel buffer over to the buffer provided by the userspace program
    foo_size = 3;
    strncpy(buff, x_axis, foo_size);
	//sprintf(buff, "%d\n", foo_size);
    //after this copy, we need to let the userspace program know how much data we put in it's buffer
    return foo_size;
}

//This is a function that will handle the file read request
static ssize_t y_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buff)
{
    //this will just copy the internal kernel buffer over to the buffer provided by the userspace program
    foo_size = 3;
    strncpy(buff, y_axis, foo_size);
	//sprintf(buff,%d\n", foo_size);
    //after this copy, we need to let the userspace program know how much data we put in it's buffer
    return foo_size;
}

//This is a function that will handle the file read request
static ssize_t z_show(struct kobject *kobj, struct kobj_attribute *attr,
        char *buff)
{
    //this will just copy the internal kernel buffer over to the buffer provided by the userspace program
    foo_size = 3;
    strncpy(buff, z_axis, foo_size);
	//sprintf(buff,%d\n", foo_size);
    //after this copy, we need to let the userspace program know how much data we put in it's buffer
    return foo_size;
}


//This is a function that will handle the file write request
static ssize_t x_store(struct  kobject *kobj, struct kobj_attribute *attr,
        const char *buff, size_t count)
{
    //this kernel object only stores FOO_SIZE_MAX bytes, figure out which is smaller, FOO_SIZE_MAX or the data written to this file from userspace
    foo_size = min(count, (size_t)FOO_SIZE_MAX);
    //now we need to copy the buffer of bytes written to this file, into x_axis to store it in the kernel space
    strncpy(x_axis, buff, foo_size);
    return count;
}



//This is a function that will handle the file write request
static ssize_t y_store(struct  kobject *kobj, struct kobj_attribute *attr,
        const char *buff, size_t count)
{
    //this kernel object only stores FOO_SIZE_MAX bytes, figure out which is smaller, FOO_SIZE_MAX or the data written to this file from userspace
    foo_size = min(count, (size_t)FOO_SIZE_MAX);
    //now we need to copy the buffer of bytes written to this file, into x_axis to store it in the kernel space
    strncpy(y_axis, buff, foo_size);
    return count;
}

//This is a function that will handle the file write request
static ssize_t z_store(struct  kobject *kobj, struct kobj_attribute *attr,
        const char *buff, size_t count)
{
    //this kernel object only stores FOO_SIZE_MAX bytes, figure out which is smaller, FOO_SIZE_MAX or the data written to this file from userspace
    foo_size = min(count, (size_t)FOO_SIZE_MAX);
    //now we need to copy the buffer of bytes written to this file, into x_axis to store it in the kernel space
    strncpy(z_axis, buff, foo_size);
    return count;
}


//This is the actual attribute definition, this is being used to define the file name, and the READ/WRITE capabilities
static struct kobj_attribute x_attribute =
    __ATTR(xaxis, S_IRUGO | S_IWUSR, x_show, x_store);
        //foo is the file name
        // S_IRUGO means it is readable by owner, group and all users
        // S_IWUSR means it is writable by the owner
        // foo_show is the function pointer to the function used to handle a file read request
        // foo_store is teh function pointer to the function used to handle a file write request
static struct kobj_attribute y_attribute =
    __ATTR(yaxis, S_IRUGO | S_IWUSR, y_show, y_store);

static struct kobj_attribute z_attribute =
    __ATTR(zaxis, S_IRUGO | S_IWUSR, z_show, z_store);

//This is an array of attributes, this only defines a single one, but you can add more
static struct attribute *attrs[] = {
    &x_attribute.attr,
    &y_attribute.attr,
    &z_attribute.attr,
    //&some_other_attribute.attr, //like this - HINT HINT
    NULL,
};

//This is an attribute group, here it is just a container for an array of attributes
static struct attribute_group attr_group = {
    .attrs = attrs,
};

static struct kobject *kobj;


static int __init accel_init(void) {
	s32 whoAmIResult;
	s32 xAxisValue;
	s32 yAxisValue;
	s32 zAxisValue;


	printk(KERN_DEBUG "accelerometer init\n");

	i2c_dev = i2c_get_adapter(1);
	if(!i2c_dev) {
		printk(KERN_INFO "FAIL: could not get i2c adapter\n");
		goto exit;
	}
	i2c_client = i2c_new_device(i2c_dev, board_info);
	if(!i2c_client) {
		printk(KERN_INFO "FAIL: could not get i2c client\n");
		goto exit;
	}

	i2c_smbus_write_byte_data(i2c_client, 0x2A, 0x01);
	whoAmIResult = i2c_smbus_read_byte_data(i2c_client, 0x0D);
	printk(KERN_DEBUG "who am I result: 0x%x\n", whoAmIResult);

	xAxisValue = i2c_smbus_read_byte_data(i2c_client, 0x01);
	printk(KERN_DEBUG "x-axis value: 0x%x\n", xAxisValue);

	yAxisValue = i2c_smbus_read_byte_data(i2c_client, 0x03);
	printk(KERN_DEBUG "y-axis value: 0x%x\n", yAxisValue);

	zAxisValue = i2c_smbus_read_byte_data(i2c_client, 0x05);
	printk(KERN_DEBUG "z-axis value: 0x%x\n", zAxisValue);


	sprintf(x_axis, "%x\n", xAxisValue);
	sprintf(y_axis, "%x\n", yAxisValue);
	sprintf(z_axis, "%x\n", zAxisValue);
	int ret;

    	kobj = kobject_create_and_add("accelerometer", kernel_kobj);
    	if (!kobj)
        	return -ENOMEM;
    	ret = sysfs_create_group(kobj, &attr_group);
    	if (ret)
        	kobject_put(kobj);

exit:
	return 0;
}

static void __exit accel_exit(void){
	printk(KERN_DEBUG "accelerometer exit\n");
	i2c_unregister_device(i2c_client);
	kobject_put(kobj);
}

module_init(accel_init);
module_exit(accel_exit);