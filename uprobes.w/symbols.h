/* symbols.h 
 * user must include auto generated ukthread.h befor symbols.h
 */

// #include <string.h> fixme

/* struct symbols is now defined in auto generated include file
struct symbol {
	unsigned long 	address ;
	char		type;
	char		name[80];
};
*/


long int find_vaddr(char *name)
{
	int i;
	for ( i=0; i<sizeof(user_symbols)/sizeof(struct symbol);i++ ){
		if ( strcmp(name,user_symbols[i].name) == 0)
			return user_symbols[i].address;
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
