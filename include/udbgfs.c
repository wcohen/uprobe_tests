#include <linux/debugfs.h>
#include <linux/slab.h>
#include <linux/stat.h>
#include <linux/string.h>
#include <linux/spinlock.h>
#include <asm/uaccess.h>


#define DATA_BUF_SIZE 1024

//#define UPROBE_DEBUGFS_DEBUG 0

static void child_exit(void);

struct data_buf_info {
	spinlock_t lock;
	size_t bytes_in_buf;
        size_t bytes_left;
	void *cur;
        void *data;
};

struct data_buf_info *print_buf;

static struct dentry  *data_file, *cleanup_file, *dir;

int test_printk(const char *fmt, ...)
{
        va_list args;
	int len;

#ifdef UPROBE_DEBUGFS_DEBUG
	printk (KERN_ERR "calling test_printk\n");
#endif
	va_start(args, fmt);

	spin_lock(&print_buf->lock);
	len = vsnprintf(print_buf->cur, print_buf->bytes_left, fmt, args);

	print_buf->cur += len;
	print_buf->bytes_in_buf += len;
	print_buf->bytes_left -= len;
	spin_unlock(&print_buf->lock);
	va_end(args);

#ifdef UPROBE_DEBUGFS_DEBUG
	printk (KERN_ERR "exiting test_printk\n");
#endif

	return len;
}


static ssize_t read_data(struct file *file,
	char __user *user_buf, size_t count, loff_t *ppos)
{

#ifdef UPROBE_DEBUGFS_DEBUG
	printk(KERN_ERR "bytes = %d\n", print_buf->bytes_in_buf);
#endif
	if ( !print_buf->bytes_in_buf )
		return 0;

	return simple_read_from_buffer(user_buf, count, ppos,
				print_buf->data, print_buf->bytes_in_buf);
}


static ssize_t write_cleanup(struct file *file,
	const char __user *user_buf, size_t count, loff_t *ppos)
{

	char buf[32];
	static int entry=0;
	if (entry)
		return count;
	entry ++;	
	if(copy_from_user(buf, user_buf,  min(count,(sizeof(buf)-1))))
		return -EINVAL;

#ifdef UPROBE_DEBUGFS_DEBUG
	printk(KERN_ERR "udfs child exit called\n");
#endif
	printk(KERN_ERR "udfs child exit called\n");
	child_exit();

	return count; 

}

static struct file_operations fops_data = {
	.read = read_data,
};

static struct file_operations fops_cleanup = {
	.write = write_cleanup,
};

/* Could just use a static define of print_buf  */
static int set_up_print_buffer(void)
{
#ifdef UPROBE_DEBUGFS_DEBUG
	printk(KERN_ERR "	set_up_print_buffer () initiated \n");
#endif
	print_buf = kmalloc(sizeof(print_buf),GFP_KERNEL);
        if ( !print_buf ){
                return -ENOMEM;
        }

        print_buf->data = NULL;
        print_buf->bytes_in_buf = 0;

        print_buf->data = kmalloc(DATA_BUF_SIZE,GFP_KERNEL);
        if( !print_buf->data){
                kfree(print_buf);
                return -ENOMEM;
        }

        print_buf->cur = print_buf->data;
        print_buf->bytes_left = DATA_BUF_SIZE;
	spin_lock_init(&print_buf->lock);
	printk(KERN_ERR "	set_up_print_buffer () completed\n");

	return 0;
}

static void clean_up_print_buffer(void)
{
	if (print_buf->data){
		kfree(print_buf->data);
		kfree(print_buf);
	}
}


int u_dbfs_init(const char *dbg_dir_path)
{

#ifdef UPROBE_DEBUGFS_DEBUG
	printk(KERN_ERR "u_dbfs_init(%s) initiated \n", dbg_dir_path);
#endif
	dir = debugfs_create_dir(dbg_dir_path, NULL);
        if (!dir) {
		printk(KERN_ERR "Can't create /debug/%s\n", dbg_dir_path);
                return -1;
        }

	data_file = debugfs_create_file("data", 0644,dir,0, &fops_data);
	if (!data_file) {
                printk(KERN_ERR "Can't create /debug/%s/data\n", dbg_dir_path);
		debugfs_remove(dir);
		return -1;
	}
	if ( set_up_print_buffer()  < 0 ){
		printk(KERN_ERR "Could not alocate print buffer\n");
		return -1;
	}
	cleanup_file = debugfs_create_file("cleanup", 0644,dir,0, &fops_cleanup);
#ifdef UPROBE_DEBUGFS_DEBUG
	printk (KERN_ERR " created /debug/%s/data\n", dbg_dir_path);
#endif
	printk (KERN_ERR " created /debug/%s/data\n", dbg_dir_path);
	return 0;
}

void u_dbfs_cleanup(void)
{
#ifdef UPROBE_DEBUGFS_DEBUG
	printk(KERN_ERR "u_dbfs_cleanup() initiated \n");
#endif
	if (data_file)
		debugfs_remove(data_file);
	if (cleanup_file)
		debugfs_remove(cleanup_file);
	if (dir)
		debugfs_remove(dir);

	clean_up_print_buffer();
#ifdef UPROBE_DEBUGFS_DEBUG
	printk(KERN_ERR "u_dbfs_cleanup() done\n");
#endif

}

