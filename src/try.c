#include<stdio.h>

#define mm 12

int test(int a);

int main()
{
  printf("%d\n", test(mm));
  return 0;
}

int test(int a)
{
  return a;
}
