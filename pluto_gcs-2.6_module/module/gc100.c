#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <asm/uaccess.h>

#ifndef SET_MODULE_OWNER
#  define SET_MODULE_OWNER(structure) /* nothing */
#endif

MODULE_LICENSE("Dual BSD/GPL");

#define BUF_SIZE 511
#define N_DEVS 16

typedef struct
{
	struct semaphore sem;
	int minor_number;
} gc100_dev;

gc100_dev * gc100_devices;

static int gc100_major;

int s_to_d_flag[N_DEVS];
int d_to_s_flag[N_DEVS];

// Try using larger buffers
char s_to_d_buf[N_DEVS][BUF_SIZE+1];
char d_to_s_buf[N_DEVS][BUF_SIZE+1];
int d_to_s_pos[N_DEVS];
int s_to_d_pos[N_DEVS];
int d_to_s_len[N_DEVS];
int s_to_d_len[N_DEVS];

ssize_t gc100_read(struct file * filp, char * buffer, size_t length, loff_t * offset)
{
	gc100_dev * dev = filp->private_data;
	int minor_number, which_driver_set;
	int bufreadsize;
	ssize_t ret = 0;

    if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;
	
	minor_number = dev->minor_number;
	which_driver_set = (int) (minor_number / 2);
	bufreadsize = min(length, BUF_SIZE);
	
    if (minor_number % 2 == 0)
	{ // s reading
		if (d_to_s_flag[which_driver_set] == 1)
		{
			bufreadsize = min(bufreadsize,(d_to_s_len[which_driver_set] - d_to_s_pos[which_driver_set]));  // max size is what's in the buffer
			bufreadsize = 1; // DEBUG

			/*if (copy_to_user(buf, &d_to_s_data[which_driver_set], 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_to_user(buffer, &d_to_s_buf[which_driver_set][d_to_s_pos[which_driver_set]], bufreadsize))
			{
				ret = -EFAULT;
				goto out;
			}
			d_to_s_pos[which_driver_set] += bufreadsize;

			// If the buffer's been emptied, it's ready for another read
			if (d_to_s_len[which_driver_set] == d_to_s_pos[which_driver_set])
			{
				d_to_s_flag[which_driver_set] = 0;
			}
			ret = bufreadsize;
		}
		else
		{
			// buffer is not ready
			ret = 0;
		}
	}
	else
	{ // d reading
		if (s_to_d_flag[which_driver_set] == 1)
		{
			bufreadsize = min(bufreadsize, (s_to_d_len[which_driver_set] - s_to_d_pos[which_driver_set]));  // max size is what's in the buffer
			bufreadsize = 1; // DEBUG

			/*if (copy_to_user(buf, &s_to_d_data[which_driver_set], 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_to_user(buffer, &s_to_d_buf[which_driver_set][s_to_d_pos[which_driver_set]], bufreadsize))
			{
				ret = -EFAULT;
				goto out;
			}
			s_to_d_pos[which_driver_set] += bufreadsize;
			if (s_to_d_len[which_driver_set] == s_to_d_pos[which_driver_set])
			{
				s_to_d_flag[which_driver_set] = 0;
			}
			ret = bufreadsize;
		}
		else
		{
			// buffer is not ready
			ret = 0;
		}
	}
	
out:
	up(&dev->sem);
	return ret;
}

ssize_t gc100_write(struct file * filp, const char * buffer, size_t length, loff_t * f_pos)
{
    gc100_dev * dev = filp->private_data;
	ssize_t ret = -ENOMEM; /* value used in "goto out" statements */

	int which_driver_set, minor_number;
	int bufwritesize;

	if (down_interruptible(&dev->sem))
		return -ERESTARTSYS;

	minor_number = dev->minor_number & 0x0f;
	which_driver_set = (int) (minor_number / 2);
	bufwritesize = min(length, BUF_SIZE);

	if (minor_number % 2 == 0)
	{
		if (s_to_d_flag[which_driver_set] == 0)
		{
			/*if (copy_from_user(&s_to_d_data[which_driver_set], buf, 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_from_user(&s_to_d_buf[which_driver_set][0], buffer, bufwritesize))
			{
				ret = -EFAULT;
				goto out;
			}
			s_to_d_flag[which_driver_set] = 1;
			s_to_d_pos[which_driver_set] = 0;
			s_to_d_len[which_driver_set] = bufwritesize;

			ret = bufwritesize;
		}
		else
		{
			// buffer is full
			ret = 0;
		}
	}
	else
	{
		if (d_to_s_flag[which_driver_set] == 0)
		{
			/*if (copy_from_user(&d_to_s_data[which_driver_set], buf, 1))
			{
				ret = -EFAULT;
				goto out;
			}*/
			if (copy_from_user(&d_to_s_buf[which_driver_set][0], buffer, bufwritesize))
			{
				ret = -EFAULT;
				goto out;
			}
			d_to_s_flag[which_driver_set] = 1;
			d_to_s_pos[which_driver_set] = 0;
			d_to_s_len[which_driver_set] = bufwritesize;
			ret = bufwritesize;
		}
		else
		{
			// buffer is full
			ret = 0;
		}
	}

out:
	up(&dev->sem);
	return ret;
}

static int gc100_device_open(struct inode * inode, struct file * filp)
{
	gc100_dev * dev;
	int num = MINOR(inode->i_rdev);

    dev = (gc100_dev *) filp->private_data;
	if (!dev)
	{
		if (num >= N_DEVS)
			return -ENODEV;
		dev = &gc100_devices[num];
		filp->private_data = dev; /* for other methods */
	}
	dev->minor_number = MINOR(inode->i_rdev);
	
	module_get(THIS_MODULE);
//	MOD_INC_USE_COUNT;
	
	return 0;
}

static int gc100_device_release(struct inode * inode, struct file * filp)
{
	module_put(THIS_MODULE);
//	MOD_DEC_USE_COUNT;
	return 0;
}

static struct file_operations gc100_fops = {
	.read = gc100_read,
	.write = gc100_write,
	.open = gc100_device_open,
	.release = gc100_device_release
};

void /*__exit*/ gc100_exit()
{
	kfree(gc100_devices);
	
	unregister_chrdev(gc100_major, "gc100");
}

int __init gc100_init()
{
	int i;
	int result = 0;

	SET_MODULE_OWNER(&gc100_fops);

	gc100_major = register_chrdev(0, "gc100", &gc100_fops);

	gc100_devices = kmalloc(N_DEVS * sizeof(gc100_dev), GFP_KERNEL);
	memset(gc100_devices, 0, N_DEVS * sizeof(gc100_dev));

	for (i = 0; i < N_DEVS; i++)
	{
		sema_init(&gc100_devices[i].sem, 1);
		d_to_s_flag[i]=0;
		s_to_d_flag[i]=0;
	}
	
	return 0;

fail:
	gc100_exit();
	return result;
}

module_init(gc100_init);
module_exit(gc100_exit);
