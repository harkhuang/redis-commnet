#include<stdio.h>
#include<stdlib.h>
int test_realloc()
{
    int i;
    int*pn=(int*)malloc(5*sizeof(int));
    printf("malloc%p\n",pn);
    for(i=0; i<5; i++)
        pn[i]=i;
    pn=(int*)realloc(pn,3*sizeof(int));
    printf("realloc%p\n",pn);

    for(i=0; i<10; i++)
        printf("%3d   ",pn[i]);
    free(pn);
    return 0;
}
#include <stdio.h>
#include <stdarg.h>
int mon_log(char* format, ...)
{
char str_tmp[50];
int i=0;
va_list vArgList;                            //定义一个va_list型的变量,这个变量是指向参数的指针.
va_start (vArgList, format);                 //用va_start宏初始化变量,这个宏的第二个参数是第一个可变参数的前一个参数,是一个固定的参数
i=_vsnprintf(str_tmp, 50, format, vArgList); //注意,不要漏掉前面的_
va_end(vArgList);

printf("str_tmp:%s\n", str_tmp);                          //用va_end宏结束可变参数的获取
return i;                                    //返回参数的字符个数中间有逗号间隔
}



/// 这个函数的主要作用就是  格式化的通过函数参数传入变量
char *make_message(const char *fmt, ...)
{
    /* 初始时假设我们只需要不超过100字节大小的空间 */
    int n, size = 100;
    char *p;
    va_list ap;
    if ( (p = (char *) malloc(size*sizeof(char))) == NULL)
    return NULL;
    while (1)
    {
        /* 尝试在申请的空间中进行打印操作 */
        va_start(ap, fmt);
        n = vsnprintf (p, size, fmt, ap);
        va_end(ap);
        /* 如果vsnprintf调用成功，返回该字符串 */
        if (n > -1 && n < size)
        return p;
        /* vsnprintf调用失败(n<0)，或者p的空间不足够容纳size大小的字符串(n>=size)，尝试申请更大的空间*/
        size *= 2; /* 两倍原来大小的空间 */
        if ((p = (char *)realloc(p, size*sizeof(char))) == NULL)
        return NULL;
    }
}
int test_va_message()
{
    /* 调用上面的函数 */
   // char* str = make_message("%d,%d,%d,%d",5,6,7,8);
   // printf("%s\n",str);
   // free(str);
    char* str1 = make_message("%s,%f,%d,%d","string",6.123132,7,8);
    printf("%s\n",str1);
    free(str1);
    /* we allocate the memory in the make_message function, so we should release it by caller(main function). */
    return 0;
}

test_mon_log(){
    int i=mon_log("%s,%d,%d,%d","asd",2,3,4);
    printf("%d\n",i);
}
int main()
{
test_mon_log();
    return 0;
}
