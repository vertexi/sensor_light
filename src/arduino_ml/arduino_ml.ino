#include"yasML.h"
#include<stdio.h>


struct parameter
{
  Matrix *sample;
  Matrix *theta1;
  Matrix *theta2;
  Matrix *a1;
  Matrix *a2;
  Matrix *Y;
  double lambda;
} my_para;
  
void setup()
{
  Serial.begin(9600);
  Serial.println("");
}

void loop()
{

}

void test_init(struct parameter temp)
{
  int m = 200, n = 6, h = 20, o = 3;
  temp.sample = constructor(m, n);
  temp.theta1 = constructor(n, h);
  temp.theta2 = constructor(h, o);
  temp.a1     = constructor(m, h);
  temp.a2     = constructor(m, o);
  temp.Y      = constructor(m, o);

  for (int i = 0; i < m; i++)
  {
    for (int j = 0; j < n; j++)
    {
      temp.sample[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }

  for (int i = 0; i < n; i++)
  {
    for (int j = 0; j < h; j++)
    {
      temp.theta1[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }

  for (int i = 0; i < h; i++)
  {
    for (int j = 0; j < o; j++)
    {
      temp.theta2[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }

  for (int i = 0; i < m; i++)
  {
    for (int j = 0; j < h; j++)
    {
      temp.a1[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }

  for (int i = 0; i < m; i++)
  {
    for (int j = 0; j < o; j++)
    {
      temp.a2[i][j] = ((double)random(1000))/((double)10000.0);
      temp.Y[i][j] = random(100) > 50 ? 1 : 0;
    }
  }
}

Matrix *sigmoid_matrix(Matrix *variable)
{
  int row_num = variable.rows;
  int col_num = variable.columns;

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
  int row_num = sample.rows;
  int col_num = sample.columns;

  Matrix *x1 = multiply(sample, theta1);
  a1 = sigmoid_matrix(x1);

  Matrix *x2 = multiply(a1, theta2);
  a2 = sigmoid_matrix(x2);

  destroy_matrix(x1);
  destroy_matrix(x2);
}

double cost_func(Matrix *a2, Matrix *Y, Matrix *theta1, Matrix *theta2, double lambda)
{
  int row_num = a2.rows;
  int col_num = a2.columns;
  Matrix *ln = constructor(row_num, col_num);

  for(int i = 0; i < row_num; i++)
  {
    for(int j = 0; j < col_num; j++)
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
  for(int i = 0; i < row_num; i++)
  {
    for(int j = 0; j < col_num; j++)
    {
      cost_res += ln->numbers[i][j];
    }
  }

  double standardlization = 0;
  row_num = theta1.rows;
  col_num = theta1.columns;

  for(int i = 1; i < row_num; i++)
  {
    for(int j = 0; j < col_num; j++)
    {
      standardlization += ((theta1->numbers[i][j])^2);
    }
  }

  row_num = theta2.rows;
  col_num = theta2.columns;

  for(int i = 1; i < row_num; i++)
  {
    for(int j = 0; j < col_num; j++)
    {
      standardlization += ((theta2->numbers[i][j])^2);
    }
  }

  double final_cost = 0;
  final_cost = 1/(double)(Y.rows)*cost_res + 1/(double)(2*Y.rows)*standardlization;

  destroy_matrix(ln);
  return final_cost;
}

Matrix **backward(Matrix *a2, Matrix *a1,Matrix *theta1, Matrix *theta2, Matrix *Y,
Matrix *sample, double lambda)
{
  float m = 0;
  m = (float)Y.rows;

  Matrix *a2_temp = clonemx(a2);
  subtract(a2_temp, Y);
  Matrix *a1_t = transpose(a1);
  Matrix *part_theta2 = multiply(a1_t, a2_temp);
  scalar_multiply(part_theta2, 1/m);

  Matrix *theta2_t = transpose(theta2);
  Matrix *part_a1 = multiply(a2_temp, theta2_t);

  Matrix *part_a1_x1 = constructor(a1.rows, a1.columns);
  for (int i = 0; i < a1.rows; i++)
  {
    for (int j = 0; j < a1.columns; j++)
    {
      part_a1_x1->numbers[i][j] = a1->numbers[i][j]*(1-a1->numbers[i][j]);
    }
  }

  for (int i = 0; i < a1.rows; i++)
  {
    for (int j = 0; j < a1.columns; j++)
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
  for (int j = 0; j < theta2_clone.columns; j++)
  {
    theta2_clone->numbers[0][j] = 0;
  }
  add(part_theta2, theta2_clone);

  Matrix *theta1_clone = clonemx(theta1);
  scalar_multiply(theta1_clone, lambda/m);
  for (int j = 0; j < theta1_clone.columns; j++)
  {
    theta1_clone->numbers[0][j] = 0;
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

void gradient_check()
{


}
