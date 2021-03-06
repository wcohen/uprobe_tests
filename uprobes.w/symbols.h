/* symbols.h 
 * user must include auto generated ukthread.h befor symbols.h
 */

// #include <string.h> fixme

/* struct symbols is now defined in auto generated include file
struct symbol {
        char            * path ;
	loff_t          offset ;
	char		* name;
};
*/

#include <linux/fs.h>
#include <linux/namei.h>

struct inode *find_inode(char *name)
{
	int i;
	for ( i=0; i<sizeof(user_symbols)/sizeof(struct symbol);i++ ){
	  if ( strcmp(name,user_symbols[i].name) == 0) {
	    struct path path;
	    struct inode *inode = NULL;
	    int ret = kern_path(user_symbols[i].path, LOOKUP_FOLLOW, &path);
	    if (ret)
	      return 0;
	    inode = igrab(path.dentry->d_inode);
	    return inode;
	  }
	}
	return 0;
}

loff_t find_offset(char *name)
{
	int i;
	for ( i=0; i<sizeof(user_symbols)/sizeof(struct symbol);i++ ){
		if ( strcmp(name,user_symbols[i].name) == 0)
			return user_symbols[i].offset;
	}
	return 0;
}

char * glob_names(char *name,int *next,int size)
{
	int i;
	for ( i=*next; i<sizeof(user_symbols)/sizeof(struct symbol);++i ){
		//if ( strstr(user_symbols[i].name,name) > 0){ fixme
		// printf("%s %s \n",user_symbols[i].name,name);
		if ( strncmp(user_symbols[i].name,name,size) == 0){
			*next = i;
			return (char *)user_symbols[i].name;
		}
	}
	return (char *)0;/* EOL */
}

// next is the loop cursor.
// @glob is the string to match to, no reg expressions, sorry.
// @found returned match
#define for_each_glob(next,glob,found)\
	for(next=0;(found=glob_names(glob,&next,strnlen(glob,20)));next++)
