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

void exp_test()
{
  unsigned long time;
  unsigned long time1;
  time = millis();
  for(double i = -200.0; i < 200.1; i += 0.1)
  {
    taylor(i);
  }
  time1 = millis();
  unsigned long temp = time1 - time;
  Serial.println(temp);

  for(double i = -200.0; i < 200.1; i += 0.1)
  {
    pow(2.71828, i);
  }
  time = millis();
  Serial.println(time - time1);
}

void error_test()
{
  unsigned long time;
  unsigned long time1;
  time = millis();
  for(double i = -10.0; i < 10.1; i += 0.01)
  {
    Serial.println(taylor(i)-pow(2.71828, i));
  }
  time1 = millis();
  unsigned long temp = time1 - time;
  Serial.println(temp);
  
}

void setup()
{
  Serial.begin(9600);
  Serial.println();
  // exp_test();
  // error_test();
  Serial.println(pow(EULER, 5));
  Serial.println(taylor(5));
}

void loop()
{

}
