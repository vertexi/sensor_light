double taylor(double x)
{
  double result = 1;
  double xx = x;

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


float means[6] = {-51.6904, -49.2619, 7.7699, 8.7001, 10.0467, 7.9498};
float divider[6] = {6.4862, 7.007, 7.7699, 8.7001, 10.0467, 7.9498};
float paras[7] = {-0.4427, 1.3809, 0.2777, 0.5639, 0.8723, 0.6008, 0.80337};

float pos_vector[6] = {0.4148, -0.3907, -1.642, 0.2545, 0.1327, -2.7523};

uint8_t positioning()
{
//  //standardlize
//  for (int i = 0; i < 6; i++)
//  {
//    pos_vector[i] -= means[i];
//    pos_vector[i] *= divide(1, divider[i]);
//  }

  //linear transform
  float linear = 0;
  for (int i = 0; i < 6; i++)
  {
    linear += (pos_vector[i]*paras[i]);
  }
  linear += paras[6];

  //logstic
  double result = 0;
  result = 1/(1+taylor(-linear));
  
  Serial.print("result:");Serial.println(result, 5);

  if (result < 0.5)
  {
    return 1;
  }else
  {
    return 2;
  }
}

void setup()
{
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
}

void loop()
{
  positioning();
  double temp = 0;
  int print_result = 0;
  temp = taylor(2.35);
  print_result = (int)(temp*1000);
  Serial.println(print_result);

}
