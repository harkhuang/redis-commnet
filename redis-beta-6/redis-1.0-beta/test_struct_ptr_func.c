struct a 
{
int b;
int c;
}; 

//结构体定义后需要加分号。
//结构体初始化方式：
struct a a1 = { 1, 2};
struct a a1 = {
.b = 1,
.c = 2
};
struct a a1 = {
b:1,
c:2
}
//Linux内核习惯用第二种，使用第二种或第三种时，结构体成员的初始化顺序可变。


/*

C语言是面向过程的，但是C语言写出的linux系统是面向对象的。
非面向对象的语言，不一定不能实现面向对象。
只是说用面向对象的语言来实现面向对象要更加简单一些、直观一些、无脑一些。
用C++、Java等面向对象的语言来实现面向对象简单一些，因为语言本身帮我们做了很多事情；
但是用C来实现面向对象很麻烦，看起来也不容易理解，
这就是为什么大多数人学过C语言却看不懂linux内核代码的原因。
如下例子：
*/


#include<stdio.h>


struct Struct_Insect_Poiter
{
int a;
int b;

void (*Out)(void);
int (*Add_To_Sum)(int , int );
}; 


void test(void);
int test_1(int a, int b);


int main(void)
{

int Sum = 0;

struct Struct_Insect_Poiter struct_a = 
{
.a = 1,
.b = 2,
.Out = test //结构体初始化末尾，不需要加任何符号.
}; //第二种结构体初始化方式。

struct Struct_Insect_Poiter struct_b =
{
a:3,
b:6,
Add_To_Sum:test_1//函数指针的初始化，将该函数指针指向与其类型匹配的函数。
}; //第三种结构体初始化方式。

Sum = struct_b.Add_To_Sum(struct_b.a, struct_b.b);//调用该函数指针。
printf("Sum = %d\n",Sum); //Sum = 9,结果正确。

// struct_a.Out;//无报错，但是无正确结果。
struct_a.Out(); //结果正确。



return 0;
}






void test(void)
{

printf("Succeed!\n");
}


int test_1(int a, int b)
{

return a+b;
}








