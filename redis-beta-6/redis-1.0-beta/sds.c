/* SDSLib, A C dynamic strings library
 *
 * Copyright (C) 2006-2009 Salvatore Sanfilippo, antirez@gmail.com
 * This softare is released under the following BSD license:
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "sds.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>



// 动态字符串
// 1.我们调用拷贝和连接的函数时候不需要考虑字符串的大小,
// 在 确定我们字符串是安全的情况下(不包含多余的0) 字符串我们
// 可以随便用  只要malloc 或realloc 分配不失败
// 2.  // TODO:请思考在多线程情况下如何来优化这个文件??
/* simple dymanic string */
// simple dymanic string Out Of Memory Abort


// 针对不能申请内存的异常处理
static void sdsOomAbort(void) {
    fprintf(stderr,"SDS: Out Of Memory (SDS_ABORT_ON_OOM defined)\n");
    abort();
}



// simple dymanic new by len
//  a variable of type struct sdshdr
sds sdsnewlen(const void *init, size_t initlen) {
    struct sdshdr *sh;

    sh = malloc(sizeof(struct sdshdr)+initlen+1);  //结构体长度+  可变字符长度+ 符 '\0'  =lenght
#ifdef SDS_ABORT_ON_OOM    //中断容错处理开启
    if (sh == NULL) sdsOomAbort();
#else
    if (sh == NULL) return NULL;
#endif
    sh->len = initlen;
    sh->free = 0;

    
    if (initlen) {
        if (init) memcpy(sh->buf, init, initlen);// 将原有字符串拷贝过来
        else memset(sh->buf,0,initlen); // 否则清空所有内容
    }
    sh->buf[initlen] = '\0';
    return (char*)sh->buf;
}


// 清空字符串,不释放内存
// 但是由于free保存了字符串可用长度,所以这个字符串可以被复用
sds sdsempty(void) {
    return sdsnewlen("",0);
}


// 初始化一个字符串 用malloc去申请内存空间
sds sdsnew(const char *init) {
    size_t initlen = (init == NULL) ? 0 : strlen(init);
    return sdsnewlen(init, initlen);
}


// 获取字符串的长度
size_t sdslen(const sds s) {
     // 减去一个值可以得到的是结构体的名字  即结构体的首地址
     //  面试题目    
     //   address1  address1   address1   address1
     //   0x0001    0x0002     0x0003     0x0004
     //   在指针前有个逻辑地址  ,  0x0004  -4  = 0x0000  得到的是结构体的名字的地址
     //    //***0x0000  就是结构体的地址*///
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    return sh->len;
}



//重定向字符串
sds sdsdup(const sds s) {
    return sdsnewlen(s, sdslen(s));
}


// 释放字符串
void sdsfree(sds s) {
    if (s == NULL) return;
    free(s-sizeof(struct sdshdr));//这里虽然释放但是没有清零  ??
}

// 获取可用字符长度
size_t sdsavail(sds s) {
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    return sh->free;//返回可用长度
}


//更新字符实际的长度和可用的长度
void sdsupdatelen(sds s) {
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    int reallen = strlen(s);
    sh->free += (sh->len-reallen);//更新实际可用长度
    sh->len = reallen;
}


// 为字符串新申请空间
// 当剩余空间大于申请值 
 //->直接返回
// 当剩余空间不大于新申请的空间
// -> realloc 一个两倍长度空间
static sds sdsMakeRoomFor(sds s, size_t addlen) {
    struct sdshdr *sh, *newsh;
    size_t free = sdsavail(s);
    size_t len, newlen;

    // IO优化
    if (free >= addlen) return s;//空闲大于申请 直接返回
    len = sdslen(s);
    sh = (void*) (s-(sizeof(struct sdshdr)));
    newlen = (len+addlen)*2;


    //重新分配内存空间  ,即使是重新申请的
    // 返回一个新的逻辑地址
    // 顺便说下realloc 函数  
    // newsize  > oldsize   添加新的内存块
    // newsize  < oldsize   改变了索引而已

  // 1）如果当前内存段后面有需要的内存空间，则直接扩展这段内存空间，realloc()将返回原指针。 
  // 2）如果当前内存段后面的空闲字节不够，那么就使用堆中的第一个能够满足这一要求的内存块，将目前的数据复制到新的位置，并将原来的数据块释放掉，返回新的内存块位置。 
  // 3）如果申请失败，将返回NULL，此时，原来的指针仍然有效。
    // IO优化
    newsh = realloc(sh, sizeof(struct sdshdr)+newlen+1);

    if (newsh == NULL) return NULL;//分配失败返回null

    newsh->free = newlen - len;
    return newsh->buf;
}


//  带有长度的连接字符串
sds sdscatlen(sds s, void *t, size_t len) {
    struct sdshdr *sh;
    size_t curlen = sdslen(s);

    // 为字符串新添加块长度
    // makeroom用来realloc进行的性能上的优化
    // 相当于基于内核IO上的堆操作连接,比用户级
    // 效率高很多       // IO优化
    s = sdsMakeRoomFor(s,len);
    // 分配失败返回null
    if (s == NULL) return NULL;
    // 获取sh的结构体名称
    sh = (void*) (s-(sizeof(struct sdshdr)));
    // 内存拷贝连接字符串
    memcpy(s+curlen, t, len);
    sh->len = curlen+len;
    sh->free = sh->free-len;
    s[curlen+len] = '\0';
    return s;
}


// 计算后面字符串的长度   做内存拼接
// 这个函数只能说调用起来方便了很多
// 但是本质离不开字符串长度的计算,在不能确定我们
// 所用字符串长度的时候这个操作还是很危险的
// 并不安全
sds sdscat(sds s, char *t) {
    return sdscatlen(s, t, strlen(t));
}


// 拷贝字符串
sds sdscpylen(sds s, char *t, size_t len) {
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    size_t totlen = sh->free+sh->len;

    // 如果总长度 小于要拷贝的字符串的长度
    // 我们realloc函数重新给字符串分配长度
    if (totlen < len) {
        // 给新字符串搞出来点空间好存放他
        // 这里给的空间不是死的  而是动态的  
        // 一个经常用的字符串可能很大 
        // 使用起来可以省去很多的free操作
        // 效率更牛 IO优化的考虑点
        s = sdsMakeRoomFor(s,len-totlen);
        if (s == NULL) return NULL;

       
        sh = (void*) (s-(sizeof(struct sdshdr)));
        //更新总长度 后面用起来更加方便
        totlen = sh->free+sh->len;
    }
    memcpy(s, t, len);
    s[len] = '\0';
    sh->len = len;
    sh->free = totlen-len;
    return s;
}

sds sdscpy(sds s, char *t) {
    return sdscpylen(s, t, strlen(t));
}


// 通过格式化的方式来写入字符串
sds sdscatprintf(sds s, const char *fmt, ...) {
    
    va_list ap;
    char *buf, *t;
    size_t buflen = 32;

    va_start(ap, fmt);
    while(1) {
        buf = malloc(buflen);
#ifdef SDS_ABORT_ON_OOM
        if (buf == NULL) sdsOomAbort();
#else
        if (buf == NULL) return NULL;
#endif
        buf[buflen-2] = '\0';

        // ...省略的是后面实际的参数,fmt其实就是确定了参数的个数
        // vsnprintf(dest,safeLen,"%s%d%x",arg1,arg2,arg3)
        vsnprintf(buf, buflen, fmt, ap);

        // 说明写完字符串之后  '\0'被覆盖了  说明字符串长度不够
        if (buf[buflen-2] != '\0') {
            free(buf);
            buflen *= 2;
            continue;
        }
        break;
    }
    va_end(ap);

    // 连接字符串
    t = sdscat(s, buf);
    free(buf);
    return t;
}

/* Remove the part of the string from left and from right composed just of
 * contiguous characters found in 'cset', that is a null terminted C string.
 *
 * After the call, the modified sds string is no longer valid and all the
 * references must be substituted with the new pointer returned by the call.
 *
 * Example:
 *
 * s = sdsnew("AA...AA.a.aa.aHelloWorld     :::");
 * s = sdstrim(s,"Aa. :");
 * printf("%s\n", s);
 *
 * Output will be just "Hello World".
 */
// 去掉头尾出现的字符串 cset
sds sdstrim(sds s, const char *cset) {
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    char *start, *end, *sp, *ep;
    size_t len;

    sp = start = s;
    ep = end = s+sdslen(s)-1;

    // 每次拿出一个字符串 与要去掉的字符串做比较  找到并且去除 
    // 这个操作只是对首位字符串进行剔除,如果在中间 仍然保持不变
    while(sp <= end && strchr(cset, *sp)) sp++;
    while(ep > start && strchr(cset, *ep)) ep--;

    // 如果从头到尾剔除 向前移动位数大于字符总长度 字符长度取0 
    // 不然字符长度 取二者相减 + 1
    len = (sp > ep) ? 0 : ((ep-sp)+1);
    if (sh->buf != sp) memmove(sh->buf, sp, len);
    sh->buf[len] = '\0';

    //为空字符设置长度
    sh->free = sh->free+(sh->len-len);
    sh->len = len;
    return s;
}

/* Turn the string into a smaller (or equal) string containing only the
 * substring specified by the 'start' and 'end' indexes.
 *
 * start and end can be negative, where -1 means the last character of the
 * string, -2 the penultimate character, and so forth.
 *
 * The interval is inclusive, so the start and end characters will be part
 * of the resulting string.
 *
 * The string is modified in-place.
 *
 * Example:
 *
 * s = sdsnew("Hello World");
 * sdsrange(s,1,-1); => "ello World"
 */
// 字符串排序么?
sds sdsrange(sds s, long start, long end) {
    struct sdshdr *sh = (void*) (s-(sizeof(struct sdshdr)));
    size_t newlen, len = sdslen(s);

    if (len == 0) return s;

    // 小于零做减法移动指针
    if (start < 0) {
        start = len+start;
        if (start < 0) start = 0;
    }
    // 小于零移动指针
    if (end < 0) {
        end = len+end;
        if (end < 0) end = 0;
    }

    // 即使输入开始位置大于结束位置  那么从0开始
    newlen = (start > end) ? 0 : (end-start)+1;
    if (newlen != 0) {
        if (start >= (signed)len) start = len-1;
        if (end >= (signed)len) end = len-1;
        newlen = (start > end) ? 0 : (end-start)+1;
    } else {
        start = 0;
    }

    // 这里用memove使得IO性能有所提升?
    if (start != 0) memmove(sh->buf, sh->buf+start, newlen);
    sh->buf[newlen] = 0;
    sh->free = sh->free+(sh->len-newlen);
    sh->len = newlen;
    return s;
}

void sdstolower(sds s) {
    int len = sdslen(s), j;

    for (j = 0; j < len; j++) s[j] = tolower(s[j]);
}

void sdstoupper(sds s) {
    int len = sdslen(s), j;

    for (j = 0; j < len; j++) s[j] = toupper(s[j]);
}

int sdscmp(sds s1, sds s2) {
    size_t l1, l2, minlen;
    int cmp;

    l1 = sdslen(s1);
    l2 = sdslen(s2);
    minlen = (l1 < l2) ? l1 : l2;
    cmp = memcmp(s1,s2,minlen);
    if (cmp == 0) return l1-l2;
    return cmp;
}

/* Split 's' with separator in 'sep'. An array
 * of sds strings is returned. *count will be set
 * by reference to the number of tokens returned.
 *
 * On out of memory, zero length string, zero length
 * separator, NULL is returned.
 *
 * Note that 'sep' is able to split a string using
 * a multi-character separator. For example
 * sdssplit("foo_-_bar","_-_"); will return two
 * elements "foo" and "bar".
 *
 * This version of the function is binary-safe but
 * requires length arguments. sdssplit() is just the
 * same function but for zero-terminated strings.
 */
sds *sdssplitlen(char *s, int len, char *sep, int seplen, int *count) {
    int elements = 0, slots = 5, start = 0, j;

    sds *tokens = malloc(sizeof(sds)*slots);
#ifdef SDS_ABORT_ON_OOM
    if (tokens == NULL) sdsOomAbort();
#endif
    if (seplen < 1 || len < 0 || tokens == NULL) return NULL;
    for (j = 0; j < (len-(seplen-1)); j++) {
        /* make sure there is room for the next element and the final one */
        if (slots < elements+2) {
            slots *= 2;
            sds *newtokens = realloc(tokens,sizeof(sds)*slots);
            if (newtokens == NULL) {
#ifdef SDS_ABORT_ON_OOM
                sdsOomAbort();
#else
                goto cleanup;
#endif
            }
            tokens = newtokens;
        }
        /* search the separator */
        if ((seplen == 1 && *(s+j) == sep[0]) || (memcmp(s+j,sep,seplen) == 0)) {
            tokens[elements] = sdsnewlen(s+start,j-start);
            if (tokens[elements] == NULL) {
#ifdef SDS_ABORT_ON_OOM
                sdsOomAbort();
#else
                goto cleanup;
#endif
            }
            elements++;
            start = j+seplen;
            j = j+seplen-1; /* skip the separator */
        }
    }
    /* Add the final element. We are sure there is room in the tokens array. */
    tokens[elements] = sdsnewlen(s+start,len-start);
    if (tokens[elements] == NULL) {
#ifdef SDS_ABORT_ON_OOM
                sdsOomAbort();
#else
                goto cleanup;
#endif
    }
    elements++;
    *count = elements;
    return tokens;

#ifndef SDS_ABORT_ON_OOM
cleanup:
    {
        int i;
        for (i = 0; i < elements; i++) sdsfree(tokens[i]);
        free(tokens);
        return NULL;
    }
#endif
}
