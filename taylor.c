#include <stdio.h>

double taylor(double x)
{
  double result = 1;
  double xx = x;

  int status = 0;
  if (x < 0)
  {
    x = -x;
    xx = x;
    status = 1;
  }
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
  xx *= x; 
  result += (xx/5040);

  if (status == 0)
  {
    return result;
  } else {
    return 1/result;
  }
}



int main()
{
  float temp_input = 0;
  scanf("%f", &temp_input);
  printf("%f\n", taylor(temp_input));
  double temp = 0;
  temp = taylor(temp_input);
  int print_result = 0;
  print_result = (int)(temp*1000);
  printf("%d", print_result);
  return 0;
}
