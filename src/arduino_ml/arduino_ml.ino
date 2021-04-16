#include"yasML.h"
#include<stdio.h>

void test_init(struct parameter temp);
Matrix *sigmoid_matrix(Matrix *variable);
double sigmoid(double num);
void feedforward(Matrix *sample, Matrix *theta1, Matrix *theta2, Matrix *a1,
Matrix *a2);
double cost_func(Matrix *a2, Matrix *Y, Matrix *theta1, Matrix *theta2, double
lambda);
Matrix **backward(Matrix *a2, Matrix *a1,Matrix *theta1, Matrix *theta2, Matrix *Y,
Matrix *sample, double lambda);
void gradient_check(struct parameter temp);

double eps = 1e-4;
struct parameter
{
  Matrix *sample;
  Matrix *theta1;
  Matrix *theta2;
  Matrix *a1;
  Matrix *a2;
  Matrix *Y;
  double lambda;
  int m;
  int n;
  int h;
  int o;
} my_para;
  
void setup()
{
  Serial.begin(9600);
  Serial.println("");
  Serial.println("setup");//debug_message
  test_init(my_para);
  //gradient_check(my_para);
  Serial.println("init_end");
}

void loop()
{

}

void test_init(struct parameter temp)
{
  int m = 200, n = 6, h = 20, o = 3;
  temp.m = 200;
  temp.n = 6;
  temp.h = 20;
  temp.o = 3;
  temp.sample = constructor(m, n);
  temp.theta1 = constructor(n, h);
  temp.theta2 = constructor(h, o);
  temp.a1     = constructor(m, h);
  temp.a2     = constructor(m, o);
  temp.Y      = constructor(m, o);
  temp.lambda = 1;

  unsigned int i = 0, j = 0;
  for (j = 0; j < m; j++)
  {
    for (i = 0; i < n; i++)
    {
      (temp.sample)->numbers[i][j] = ((double)random(1000))/((double)10000.0); 
    }
  }
  Serial.println("sample_init");//debug_message

  for (j = 0; j < n; j++)
  {
    for (i = 0; i < h; i++)
    {
      temp.theta1->numbers[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }
  Serial.println("theta1_init");//debug_message

  for (j = 0; j < h; j++)
  {
    for (i = 0; i < o; i++)
    {
      temp.theta2->numbers[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }
  Serial.println("theta2_init");//debug_message

  for (j = 0; j < m; j++)
  {
    for (i = 0; i < o; i++)
    {
      (temp.Y)->numbers[i][j] = (double)random(100);
      Serial.print((temp.Y)->numbers[i][j]);
      Serial.print("\n");
    }
  }

  for (j = 0; j < m; j++)
  {
    for (i = 0; i < o; i++)
    {
      Serial.print((temp.Y)->numbers[i][j]);
      Serial.print(" ");
    }
      Serial.print("\n");
  }
  Serial.println("Y_init");//debug_message
  return;
}

Matrix *sigmoid_matrix(Matrix *variable)
{
  int row_num = variable->rows;
  int col_num = variable->columns;

  Matrix *res = constructor(row_num, col_num);
  for(int i = 0; i < row_num; i++)
  {
    for(int j = 0; j < col_num; j++)
    {
      res->numbers[i][j] = sigmoid(variable->numbers[i][j]);
    }
  }

  return res;
}

double sigmoid(double num)
{
  return (1/(1+pow(EULER, -num)));
}

void feedforward(Matrix *sample, Matrix *theta1, Matrix *theta2, Matrix *a1,
Matrix *a2)
{
  int row_num = sample->rows;
  int col_num = sample->columns;

  Matrix *x1 = multiply(sample, theta1);
  a1 = sigmoid_matrix(x1);

  Matrix *x2 = multiply(a1, theta2);
  a2 = sigmoid_matrix(x2);

  destroy_matrix(x1);
  destroy_matrix(x2);
}

double cost_func(Matrix *a2, Matrix *Y, Matrix *theta1, Matrix *theta2, double lambda)
{
  int row_num = a2->rows;
  int col_num = a2->columns;
  Matrix *ln = constructor(row_num, col_num);

  for(int j = 0; j < row_num; j++)
  {
    for(int i = 0; i < col_num; i++)
    {
      if (Y->numbers[i][j] == 1)
      {
        ln->numbers[i][j] = -log(a2->numbers[i][j]);
      } else if (Y->numbers[i][j] == 0)
      {
        ln->numbers[i][j] = -log(1-a2->numbers[i][j]);
      }
    }
  }

  double cost_res = 0;
  for(int j = 0; j < row_num; j++)
  {
    for(int i = 0; i < col_num; i++)
    {
      cost_res += ln->numbers[i][j];
    }
  }

  double standardlization = 0;
  row_num = theta1->rows;
  col_num = theta1->columns;

  for(int j = 1; j < row_num; j++)
  {
    for(int i = 0; i < col_num; i++)
    {
      standardlization += ((theta1->numbers[i][j])*(theta1->numbers[i][j]));
    }
  }

  row_num = theta2->rows;
  col_num = theta2->columns;

  for(int j = 1; j < row_num; j++)
  {
    for(int i = 0; i < col_num; i++)
    {
      standardlization += ((theta2->numbers[i][j])*(theta2->numbers[i][j]));
    }
  }

  double final_cost = 0;
  final_cost = 1/(double)(Y->rows)*cost_res + 1/(double)(2*Y->rows)*standardlization;

  destroy_matrix(ln);
  return final_cost;
}

Matrix **backward(Matrix *a2, Matrix *a1,Matrix *theta1, Matrix *theta2, Matrix *Y,
Matrix *sample, double lambda)
{
  float m = 0;
  m = (float)Y->rows;

  Matrix *a2_temp = clonemx(a2);
  subtract(a2_temp, Y);
  Matrix *a1_t = transpose(a1);
  Matrix *part_theta2 = multiply(a1_t, a2_temp);
  scalar_multiply(part_theta2, 1/m);

  Matrix *theta2_t = transpose(theta2);
  Matrix *part_a1 = multiply(a2_temp, theta2_t);

  Matrix *part_a1_x1 = constructor(a1->rows, a1->columns);
  for (int j = 0; j < a1->rows; j++)
  {
    for (int i = 0; i < a1->columns; i++)
    {
      part_a1_x1->numbers[i][j] = a1->numbers[i][j]*(1-a1->numbers[i][j]);
    }
  }

  for (int j = 0; j < a1->rows; j++)
  {
    for (int i = 0; i < a1->columns; i++)
    {
      part_a1->numbers[i][j] =
      (part_a1_x1->numbers[i][j])*(part_a1->numbers[i][j]);
    }
  }

  Matrix *X_t = transpose(sample);
  Matrix *part_theta1 = multiply(X_t, part_a1);
  scalar_multiply(part_theta1, 1/m);

  //standarndlization gradient
  Matrix *theta2_clone = clonemx(theta2);
  scalar_multiply(theta2_clone, lambda/m);
  for (int j = 0; j < theta2_clone->columns; j++)
  {
    theta2_clone->numbers[j][0] = 0;
  }
  add(part_theta2, theta2_clone);

  Matrix *theta1_clone = clonemx(theta1);
  scalar_multiply(theta1_clone, lambda/m);
  for (int j = 0; j < theta1_clone->columns; j++)
  {
    theta1_clone->numbers[j][0] = 0;
  }
  add(part_theta1, theta1_clone);

  destroy_matrix(a1_t);
  destroy_matrix(a2_temp);
  destroy_matrix(theta2_t);
  destroy_matrix(part_a1);
  destroy_matrix(part_a1_x1);
  destroy_matrix(X_t);
  destroy_matrix(theta2_clone);
  destroy_matrix(theta1_clone);

  Matrix **gradient_res = malloc(sizeof(Matrix *)*2);
  gradient_res[0] = part_theta1;
  gradient_res[1] = part_theta2;

  return gradient_res;
}

void gradient_check(struct parameter temp)
{
  double cost1 = 0;
  double cost2 = 0;

  Matrix *numerical_g_th1 = constructor(temp.n, temp.h);
  Matrix *numerical_g_th2 = constructor(temp.h, temp.o);

  Matrix *theta_1_min = clonemx(temp.theta1);
  for (int j = 0; j < temp.n; j++)
  {
    for (int i = 0; i < temp.h; i++)
    {
      theta_1_min->numbers[i][j] -= eps;
      cost1 = cost_func(temp.a2, temp.Y, theta_1_min, temp.theta2, temp.lambda);
      theta_1_min->numbers[i][j] += (2*eps);
      cost2 = cost_func(temp.a2, temp.Y, theta_1_min, temp.theta2, temp.lambda);
      numerical_g_th1->numbers[i][j] = (cost2 - cost1)/(2*eps);
      theta_1_min->numbers[i][j] -= eps;
      Serial.println(i);
    }
  }
  Serial.println("ofcoursefuckI'm working");

  Matrix *theta_2_min = clonemx(temp.theta2);
  for (int j = 0; j < temp.h; j++)
  {
    for (int i = 0; i < temp.o; i++)
    {
      theta_2_min->numbers[i][j] -= eps;
      cost1 = cost_func(temp.a2, temp.Y, temp.theta1, theta_2_min, temp.lambda);
      theta_2_min->numbers[i][j] += (2*eps);
      cost2 = cost_func(temp.a2, temp.Y, temp.theta1, theta_2_min, temp.lambda);
      numerical_g_th2->numbers[i][j] = (cost2 - cost1)/(2*eps);
      theta_2_min->numbers[i][j] -= eps;
    }
  }

  destroy_matrix(theta_1_min);
  destroy_matrix(theta_2_min);
  Serial.println("I'm working");

  Matrix **analysis_grad = backward(temp.a2, temp.a1, temp.theta1, temp.theta2,
  temp.Y, temp.sample, temp.lambda);

  //print(numerical_g_th1);
  Serial.println("ana");
  //print(analysis_grad[0]);
  Serial.println("end1");

  //print(numerical_g_th2);
  Serial.println("ana");
  //print(analysis_grad[1]);
  Serial.println("end2");

  subtract(analysis_grad[0], numerical_g_th1);
  double diff = 0;
  for (int j = 0; j < temp.n; j++)
  {
    for (int i = 0; i < temp.h; i++)
    {
      diff +=
      ((analysis_grad[0])->numbers[i][j])*((analysis_grad[0])->numbers[i][j]);
    }
  }
  diff = sqrt(diff/(double)(temp.n*temp.h));
  Serial.println("theta1 gradient error:");
  Serial.println(diff);

  subtract(analysis_grad[1], numerical_g_th2);
  diff = 0;
  for (int j = 0; j < temp.h; j++)
  {
    for (int i = 0; i < temp.o; i++)
    {
      diff +=
      ((analysis_grad[1])->numbers[i][j])*((analysis_grad[1])->numbers[i][j]);
    }
  }
  diff = sqrt(diff/(double)(temp.h*temp.o));
  Serial.println("theta2 gradient error:");
  Serial.println(diff);
}