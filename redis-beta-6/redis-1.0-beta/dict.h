/* Hash Tables Implementation - Copyright (C) 2006-2008 Salvatore Sanfilippo
 * antirez at gmail dot com
 *
 * Please see dict.c for more information
 */

#ifndef __DICT_H
#define __DICT_H

#define DICT_OK 0
#define DICT_ERR 1


/*--一些关于hash表实现的疑问--*/
/*-
1.哈希表中 存放的是hash值的映射关系
2.哈希表中 存放的1-16个元素的逻辑位置
3.在表不够用时候如何实现的扩展?
4.关注hash实现映射所用的函数
-*/



/* Unused arguments generate annoying warnings... */
#define DICT_NOTUSED(V) ((void) V)


// 哈希入口  其中的下一个指针让我感到困惑
typedef struct dictEntry {
    void *key;
    void *val;
    
    // 基于逻辑索引,这个向下的指针只能用于遍历,
    // 除此之外还有什么作用呢?
    struct dictEntry *next;

} dictEntry;

// 封装了哈希调用的全部方法  这里用的都是堆空间
// 这里封装的函数指针何时给予赋值操作?
typedef struct dictType {
    unsigned int (*hashFunction)(const void *key);
    void *(*keyDup)(void *privdata, const void *key);
    void *(*valDup)(void *privdata, const void *obj);
    int (*keyCompare)(void *privdata, const void *key1, const void *key2);
    void (*keyDestructor)(void *privdata, void *key);
    void (*valDestructor)(void *privdata, void *obj);
} dictType;


// 整体我们称之为哈希表

// 整体是哈希表    单体是哈希的单个元素
typedef struct dict {
    // 真正的哈希数据
    // 这是个非常牛逼的二级指针
    // 除去了地址的限制
    // 使得数据逻辑地址变得连续

    dictEntry **table;// 单链表哈希数据存储

    // 基于数据优化的一些参数
    dictType *type; //哈希内部操作函数
    unsigned int size;      // 大小?
    //??   用位与操作 获取index  即哈希值的位置
    //  那么哈希值关联了相对位移地址
    // 通过指针加上相对位移就可以获取到value
    // maske 如何和大小相关联的
    unsigned int sizemask; 
    unsigned int used; //未使用的hash空间
    void *privdata;   //私有数据?存放什么?
} dict;

// 一个哈希表的迭代器指针
typedef struct dictIterator {
    // 哈希表指针
    dict *ht;// 哈希表指针
    int index;//索引项
    dictEntry *entry, *nextEntry;//当前项和下一项
} dictIterator;

/* This is the initial size of every hash table */
#define DICT_HT_INITIAL_SIZE     16

/* ------------------------------- Macros ------------------------------------*/
#define dictFreeEntryVal(ht, entry) \
    if ((ht)->type->valDestructor) \
        (ht)->type->valDestructor((ht)->privdata, (entry)->val)
// 如果函数指针不为空  那么调用指针释放哈希值





#define dictSetHashVal(ht, entry, _val_) do { \
    if ((ht)->type->valDup) \
        entry->val = (ht)->type->valDup((ht)->privdata, _val_); \
    else \
        entry->val = (_val_); \
} while(0)
// 哈希重定向指针不为空,调用哈希重定向指针,如果哈希重定向指针为空,仅仅设置哈希值


#define dictFreeEntryKey(ht, entry) \
    if ((ht)->type->keyDestructor) \
        (ht)->type->keyDestructor((ht)->privdata, (entry)->key)
//释放hashkey


#define dictSetHashKey(ht, entry, _key_) do { \
    if ((ht)->type->keyDup) \
        entry->key = (ht)->type->keyDup((ht)->privdata, _key_); \
    else \
        entry->key = (_key_); \
} while(0)
// 设置哈希key


#define dictCompareHashKeys(ht, key1, key2) \
    (((ht)->type->keyCompare) ? \
        (ht)->type->keyCompare((ht)->privdata, key1, key2) : \
        (key1) == (key2))
// 比较哈希key 如果key1= key1  返回比较结果



#define dictHashKey(ht, key) (ht)->type->hashFunction(key)

#define dictGetEntryKey(he) ((he)->key)
#define dictGetEntryVal(he) ((he)->val)
#define dictGetHashTableSize(ht) ((ht)->size)
#define dictGetHashTableUsed(ht) ((ht)->used)

/* API */
dict *dictCreate(dictType *type, void *privDataPtr);
int dictExpand(dict *ht, unsigned int size);
int dictAdd(dict *ht, void *key, void *val);
int dictReplace(dict *ht, void *key, void *val);
int dictDelete(dict *ht, const void *key);
int dictDeleteNoFree(dict *ht, const void *key);
void dictRelease(dict *ht);
dictEntry * dictFind(dict *ht, const void *key);
int dictResize(dict *ht);
dictIterator *dictGetIterator(dict *ht);
dictEntry *dictNext(dictIterator *iter);
void dictReleaseIterator(dictIterator *iter);
dictEntry *dictGetRandomKey(dict *ht);
void dictPrintStats(dict *ht);
unsigned int dictGenHashFunction(const unsigned char *buf, int len);

/* Hash table types */
extern dictType dictTypeHeapStringCopyKey;
extern dictType dictTypeHeapStrings;
extern dictType dictTypeHeapStringCopyKeyValue;

#endif /* __DICT_H */
