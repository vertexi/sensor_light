#include <stdio.h>

float taylor(float x)
{
  float result = 1;
  float xx = x;

  result += x;
  xx *= x; 
  result += (xx/2);
  xx *= x; 
  result += (xx/6);
  xx *= x; 
  result += (xx/24);
  xx *= x; 
  result += (xx/120);
  xx *= x; 
  result += (xx/720);

  return result;
}



int main()
{
  printf("%f", taylor(2.35));

  return 0;
}
