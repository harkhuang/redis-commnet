/* SDSLib, A C dynamic strings library


 Redis字符串的实现包含在sds.c（sds代表简单动态字符串）中。 
 sdshdr声明的C结构sds.h表示Redis字符串： 
 struct sdshdr {
	 long len;
	 long free;
	 char buf[];
 };

 
 typedef char *sds; 
 sdsnewlen定义的函数sds.c创建一个新的Redis字符串：
 sds sdsnewlen(const void *init, size_t initlen) {
	 struct sdshdr *sh; 
	 sh = zmalloc(sizeof(struct sdshdr)+initlen+1);
#ifdef SDS_ABORT_ON_OOM
	 if (sh == NULL) sdsOomAbort();
#else
	 if (sh == NULL) return NULL;
#endif
	 sh->len = initlen;
	 sh->free = 0;
	 if (initlen) {
		 if (init) memcpy(sh->buf, init, initlen);
		 else memset(sh->buf,0,initlen);
	 }
	 sh->buf[initlen] = '\0';
	 return (char*)sh->buf;
 }
 请记住，Redis字符串是类型的变量struct sdshdr。但sdsnewlen返回一个字符指针!!
 
 这是一个技巧，需要一些解释。
 
 假设我使用sdsnewlen如下所示创建Redis字符串：
 
 sdsnewlen("redis", 5);
 这struct sdshdr将为 字段len和free字段以及buf字符数组创建一个新的类型分配内存变量。
 
 sh = zmalloc(sizeof(struct sdshdr)+initlen+1); // initlen is length of init argument.
 之后sdsnewlen成功地创建一个Redis的字符串，结果是这样的：
 
 -----------
 |5|0|redis|
 -----------
 ^	 ^
 sh  sh->buf
 sdsnewlen返回sh->buf给调用者。
 
 如果你需要释放指向的Redis字符串，你会怎么做sh？
 你想要指针，sh但你只有指针sh->buf。 
 你能得到的指针sh从sh->buf？
 
 是。指针算术。从上面的ASCII艺术中注意到，如果从中减去两个long的大小，则sh->buf得到指针sh。
 
 在sizeof二次长恰好是大小struct sdshdr。
 
 看看sdslen功能并看到这个技巧：
 
 size_t sdslen(const sds s) {
	 struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
	 return sh->len;
 }
 知道了这个技巧，你可以很容易地完成其余的功能sds.c。
 
 Redis字符串实现隐藏在只接受字符指针的接口后面。Redis字符串的用户无需关心其实现方式，并将Redis字符串视为字符指针。

 */

#ifndef __SDS_H
#define __SDS_H

#include <sys/types.h>

typedef char *sds;


/*
该buf字符数组存储实际的字符串。 
该len字段存储长度buf。这使得获得Redis字符串的长度为O（1）操作。 
该free字段存储可供使用的附加字节数。 
可以将len和free字段一起视为保存buf字符数组的元数据。 
创建Redis字符串 名为的新数据类型sds定义sds.h为字符指针的同义词：

*/

// 这个字符串的定义 free起到了非常牛逼的的作用,对于不停重复的申请内容操作可以大大的节省空间
struct sdshdr {
    long len;
    long free;
    char buf[0];
};

sds sdsnewlen(const void *init, size_t initlen);
sds sdsnew(const char *init);
sds sdsempty();
size_t sdslen(const sds s);
sds sdsdup(const sds s);
void sdsfree(sds s);
size_t sdsavail(sds s);
sds sdscatlen(sds s, void *t, size_t len);
sds sdscat(sds s, char *t);
sds sdscpylen(sds s, char *t, size_t len);
sds sdscpy(sds s, char *t);
sds sdscatprintf(sds s, const char *fmt, ...);
sds sdstrim(sds s, const char *cset);
sds sdsrange(sds s, long start, long end);
void sdsupdatelen(sds s);
int sdscmp(sds s1, sds s2);
sds *sdssplitlen(char *s, int len, char *sep, int seplen, int *count);
void sdstolower(sds s);

#endif
