/* SDSLib, A C dynamic strings library


 Redis�ַ�����ʵ�ְ�����sds.c��sds����򵥶�̬�ַ������С� 
 sdshdr������C�ṹsds.h��ʾRedis�ַ����� 
 struct sdshdr {
	 long len;
	 long free;
	 char buf[];
 };

 
 typedef char *sds; 
 sdsnewlen����ĺ���sds.c����һ���µ�Redis�ַ�����
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
 ���ס��Redis�ַ��������͵ı���struct sdshdr����sdsnewlen����һ���ַ�ָ��!!
 
 ����һ�����ɣ���ҪһЩ���͡�
 
 ������ʹ��sdsnewlen������ʾ����Redis�ַ�����
 
 sdsnewlen("redis", 5);
 ��struct sdshdr��Ϊ �ֶ�len��free�ֶ��Լ�buf�ַ����鴴��һ���µ����ͷ����ڴ������
 
 sh = zmalloc(sizeof(struct sdshdr)+initlen+1); // initlen is length of init argument.
 ֮��sdsnewlen�ɹ��ش���һ��Redis���ַ���������������ģ�
 
 -----------
 |5|0|redis|
 -----------
 ^	 ^
 sh  sh->buf
 sdsnewlen����sh->buf�������ߡ�
 
 �������Ҫ�ͷ�ָ���Redis�ַ����������ô��sh��
 ����Ҫָ�룬sh����ֻ��ָ��sh->buf�� 
 ���ܵõ���ָ��sh��sh->buf��
 
 �ǡ�ָ���������������ASCII������ע�⵽��������м�ȥ����long�Ĵ�С����sh->buf�õ�ָ��sh��
 
 ��sizeof���γ�ǡ���Ǵ�Сstruct sdshdr��
 
 ����sdslen���ܲ�����������ɣ�
 
 size_t sdslen(const sds s) {
	 struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
	 return sh->len;
 }
 ֪����������ɣ�����Ժ����׵��������Ĺ���sds.c��
 
 Redis�ַ���ʵ��������ֻ�����ַ�ָ��Ľӿں��档Redis�ַ������û����������ʵ�ַ�ʽ������Redis�ַ�����Ϊ�ַ�ָ�롣

 */

#ifndef __SDS_H
#define __SDS_H

#include <sys/types.h>

typedef char *sds;


/*
��buf�ַ�����洢ʵ�ʵ��ַ����� 
��len�ֶδ洢����buf����ʹ�û��Redis�ַ����ĳ���ΪO��1�������� 
��free�ֶδ洢�ɹ�ʹ�õĸ����ֽ����� 
���Խ�len��free�ֶ�һ����Ϊ����buf�ַ������Ԫ���ݡ� 
����Redis�ַ��� ��Ϊ������������sds����sds.hΪ�ַ�ָ���ͬ��ʣ�

*/
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
