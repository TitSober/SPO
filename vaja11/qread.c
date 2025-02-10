#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/slab.h> /* kmalloc() */
#include <linux/errno.h> /* kode napak */
#include <linux/types.h> /* size_t */
#include <linux/fcntl.h> /* O_ACCMODE */



#define DEVICE_NAME "ogrodje"

MODULE_LICENSE("GPL");

int counter;
int zacni_modul(void);
void koncaj_modul(void);
int odpri(struct inode *, struct file *);
int sprosti(struct inode *, struct file *);
ssize_t beri(struct file *, char *, size_t, loff_t *);
ssize_t pisi(struct file *, const char *, size_t, loff_t *);
int trim(char *);

int Major;
int size;
char *data;
int BLK_SIZE=32;
int MEM_SIZE=128;
struct file_operations fops = {
	.open = odpri,
	.release = sprosti,
	.read = beri,
	.write = pisi
};

module_init(zacni_modul);
module_exit(koncaj_modul);

int zacni_modul(void)
{
	Major = register_chrdev(0, DEVICE_NAME, &fops);
	if (Major < 0) {
		printk(KERN_ALERT "Registracija znakovne naprave spodletela.\n");
		return Major;
	}

	printk(KERN_INFO "Glavno stevilo je %d.\n", Major);
	size = 0;
	return 0;
}

void koncaj_modul(void)
{
	trim(data);
	size = 0;
	unregister_chrdev(Major, DEVICE_NAME);
}

int odpri(struct inode *inode, struct file *filp)
{ 
	filp->private_data = data;
	if ( (filp->f_flags & O_ACCMODE) == O_WRONLY)
	trim(data);
	
	return 0;
}

int sprosti(struct inode *inode, struct file *file)
{ 
	return 0;
}

int trim(char *data){
	if(data){
		kfree(data);
		return 0;
	}
	return -ENOMEM;
}

ssize_t beri(struct file *filp, char __user *buff, size_t len, loff_t *offset)
{
    int LocalBlockSize = BLK_SIZE; //32
	
	
	ssize_t retval = 0;
	printk("Read-Zacetek: *f_pos: %ld, count: %d, size: %d. ", *offset, len, size);
	if(*offset >=size){ //če offset kaže dlje od konca podatkov v pomnilniku izhod
		goto out;
	}
	if(len > size - *offset){ // preverja da je dolžina katero hoče program brat manjša kot je ostanek podatka v ramu
		len = size - *offset; // nastavi da bere do konca
	}
	
	//s_pos = (long)*offset / LocalBlockSize;
	
	if(len > LocalBlockSize){ //
		len = LocalBlockSize;
	}
	if (copy_to_user(buff,data+*offset,len)){	retval =-EFAULT;goto out;}

	*offset += len;
	retval = len;
	printk("Konec: *f_pos: %ld, count: %d, size: %d. ", *offset, len, size);

	out:
	return retval;
}

ssize_t pisi(struct file *filp, const char __user *buff, size_t len, loff_t *offset)
{	
	int Q = MEM_SIZE;
	

	
	ssize_t retval = -ENOMEM;
	printk("Write-Zacetek: *f_pos: %ld, count: %d, size: %d. ", *offset, len, size);
	
	if(!data){ // Če ni alociranega pomnilnika ga alociraj.
		data = kmalloc(MEM_SIZE*sizeof(char), GFP_KERNEL);
		if(!data){goto out;}
		memset(data,0,MEM_SIZE*sizeof(char));
	}
	//podatki ki se pišejo
	if(*offset>=Q){goto out;} //ne podpiramo pisanja več B kot je MEM_SIZE
	if(len > Q - *offset){len=Q - *offset;} //PIŠI SAM DO KONCA DODELJENEGA PROSTORA!
	if(copy_from_user(data + *offset, buff, len)){retval = -EFAULT;goto out;}
	*offset += len;
	size+=len;
	retval = len;
	//if(size < *offset){size=*offset;}
	

	
	printk("Konec: *f_pos: %ld, count: %d, size: %d.\n", *offset, len, size);


	out:
	return retval;
}