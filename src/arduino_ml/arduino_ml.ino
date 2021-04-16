#include<math.h>

#define NN_m 20
#define NN_n 6
#define NN_h 15
#define NN_o 3

double eps = 1e-4;
double sample[NN_m][NN_n];
double theta1[NN_n][NN_h];
double theta2[NN_h][NN_o];
double a1[NN_m][NN_h];
double a2[NN_m][NN_o];
double Y[NN_m][NN_o];
double lambda;
  
double numerical_g_th1[NN_n][NN_h];
double numerical_g_th2[NN_h][NN_o];

void setup()
{
  Serial.begin(9600);
  Serial.println("");
  Serial.println("setup");//debug_message
  test_init();
  //gradient_check(my_para);
  Serial.println("init_end");
  double cost_rr = cost_func();
  Serial.println(cost_rr);
  Serial.println("cost!!");

  print_m((double *)sample, NN_m, NN_n);
  Serial.println("theta1");
  print_m((double *)theta1, NN_n, NN_h);

  Serial.println("multiply");
  multiply((double *)sample, NN_m, NN_n, (double *)theta1, NN_n, NN_h, (double *)a1);
  print_m((double *)a1, NN_m, NN_h);
}

void loop()
{

}

void test_init()
{
  lambda = 1.0;

  unsigned int i = 0, j = 0;
  for (i = 0; i < NN_m; i++)
  {
    for (j = 0; j < NN_n; j++)
    {
      sample[i][j] = ((double)random(1000))/((double)100.0); 
    }
  }
  Serial.println("sample_init");//debug_message

  for (i = 0; i < NN_n; i++)
  {
    for (j = 0; j < NN_h; j++)
    {
      theta1[i][j] = ((double)random(1000))/((double)100.0);
    }
  }
  Serial.println("theta1_init");//debug_message

  for (i = 0; i < NN_h; i++)
  {
    for (j = 0; j < NN_o; j++)
    {
      theta2[i][j] = ((double)random(1000))/((double)10000.0);
    }
  }
  Serial.println("theta2_init");//debug_message

  for (i = 0; i < NN_m; i++)
  {
    for (j = 0; j < NN_o; j++)
    {
      Y[i][j] = (double)random(100);
    }
  }
  for (i = 0; i < NN_m; i++)
  {
    for (j = 0; j < NN_o; j++)
    {
      Y[i][j] = Y[i][j] > 50 ? 1 : 0;
    }
  }
  Serial.println("Y_init");//debug_message

  return;
}

//Matrix *sigmoid_matrix(Matrix *variable)
//{
//  int row_num = variable->rows;
//  int col_num = variable->columns;
//
//  Matrix *res = constructor(row_num, col_num);
//  for(int i = 0; i < row_num; i++)
//  {
//    for(int j = 0; j < col_num; j++)
//    {
//      res->numbers[i][j] = sigmoid(variable->numbers[i][j]);
//    }
//  }
//
//  return res;
//}
//
//double sigmoid(double num)
//{
//  return (1/(1+pow(EULER, -num)));
//}

/*
void feedforward()
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
*/

double cost_func()
{
  double ln[NN_m][NN_o];

  unsigned int i = 0, j = 0;
  for(i = 0; i < NN_m; i++)
  {
    for(j = 0; j < NN_o; j++)
    {
      if (Y[i][j] > 0.1)
      {
        ln[i][j] = -log(a2[i][j]);
      } else
      {
        ln[i][j] = -log(1-a2[i][j]);
      }
    }
  }

  double cost_res = 0;
  for(i = 0; i < NN_m; i++)
  {
    for(j = 0; j < NN_o; j++)
    {
      cost_res += ln[i][j];
    }
  }

  double standardlization = 0;
  for(i = 1; i < NN_n; i++)
  {
    for(j = 0; j < NN_h; j++)
    {
      standardlization += ((theta1[i][j])*(theta1[i][j]));
    }
  }

  for(i = 1; i < NN_h; i++)
  {
    for(j = 0; j < NN_o; j++)
    {
      standardlization += ((theta2[i][j])*(theta2[i][j]));
    }
  }

  double final_cost = 0;
  final_cost = 1/(double)(NN_m)*cost_res + 1/(double)(2*NN_m)*standardlization;

  return final_cost;
}

//Matrix **backward(Matrix *a2, Matrix *a1,Matrix *theta1, Matrix *theta2, Matrix *Y,
//Matrix *sample, double lambda)
//{
//  float m = 0;
//  m = (float)Y->rows;
//
//  Matrix *a2_temp = clonemx(a2);
//  subtract(a2_temp, Y);
//  Matrix *a1_t = transpose(a1);
//  Matrix *part_theta2 = multiply(a1_t, a2_temp);
//  scalar_multiply(part_theta2, 1/m);
//
//  Matrix *theta2_t = transpose(theta2);
//  Matrix *part_a1 = multiply(a2_temp, theta2_t);
//
//  Matrix *part_a1_x1 = constructor(a1->rows, a1->columns);
//  for (int j = 0; j < a1->rows; j++)
//  {
//    for (int i = 0; i < a1->columns; i++)
//    {
//      part_a1_x1->numbers[i][j] = a1->numbers[i][j]*(1-a1->numbers[i][j]);
//    }
//  }
//
//  for (int j = 0; j < a1->rows; j++)
//  {
//    for (int i = 0; i < a1->columns; i++)
//    {
//      part_a1->numbers[i][j] =
//      (part_a1_x1->numbers[i][j])*(part_a1->numbers[i][j]);
//    }
//  }
//
//  Matrix *X_t = transpose(sample);
//  Matrix *part_theta1 = multiply(X_t, part_a1);
//  scalar_multiply(part_theta1, 1/m);
//
//  //standarndlization gradient
//  Matrix *theta2_clone = clonemx(theta2);
//  scalar_multiply(theta2_clone, lambda/m);
//  for (int j = 0; j < theta2_clone->columns; j++)
//  {
//    theta2_clone->numbers[j][0] = 0;
//  }
//  add(part_theta2, theta2_clone);
//
//  Matrix *theta1_clone = clonemx(theta1);
//  scalar_multiply(theta1_clone, lambda/m);
//  for (int j = 0; j < theta1_clone->columns; j++)
//  {
//    theta1_clone->numbers[j][0] = 0;
//  }
//  add(part_theta1, theta1_clone);
//
//  destroy_matrix(a1_t);
//  destroy_matrix(a2_temp);
//  destroy_matrix(theta2_t);
//  destroy_matrix(part_a1);
//  destroy_matrix(part_a1_x1);
//  destroy_matrix(X_t);
//  destroy_matrix(theta2_clone);
//  destroy_matrix(theta1_clone);
//
//  Matrix **gradient_res = malloc(sizeof(Matrix *)*2);
//  gradient_res[0] = part_theta1;
//  gradient_res[1] = part_theta2;
//
//  return gradient_res;
//}

//void gradient_check()
//{
//  double cost1 = 0;
//  double cost2 = 0;
//
//  double theta_1_min[NN_n][NN_h];
//  unsigned int i = 0, j = 0;
//
//  for (i = 0; i < NN_n; i++)
//  {
//    for (j = 0; j < NN_h; j++)
//    {
//      theta_1_min[i][j] = theta1[i][j];
//    }
//  }
//
//  for (i = 0; i < NN_n; i++)
//  {
//    for (j = 0; j < NN_h; j++)
//    {
//      theta_1_min[i][j] -= eps;
//      cost1 = cost_func(a2, Y, theta_1_min, theta2, lambda);
//      theta_1_min[i][j] += (2*eps);
//      cost2 = cost_func(a2, Y, theta_1_min, theta2, lambda);
//      numerical_g_th1[i][j] = (cost2 - cost1)/(2*eps);
//      theta_1_min[i][j] -= eps;
//    }
//  }
//  Serial.println("ofcoursefuckI'm working");
//
//  double theta_2_min[NN_h][NN_o];
//  for (i = 0; i < NN_h; i++)
//  {
//    for (j = 0; j < NN_o; j++)
//    {
//      theta_2_min[i][j] -= eps;
//      cost1 = cost_func(a2, Y, theta1, theta_2_min, lambda);
//      theta_2_min[i][j] += (2*eps);
//      cost2 = cost_func(a2, Y, theta1, theta_2_min, lambda);
//      numerical_g_th2[i][j] = (cost2 - cost1)/(2*eps);
//      theta_2_min[i][j] -= eps;
//    }
//  }
//  Serial.println("I'm working");
//
//  Matrix **analysis_grad = backward(a2, a1, theta1, theta2, Y, sample, lambda);
//
//  //print(numerical_g_th1);
//  Serial.println("ana");
//  //print(analysis_grad[0]);
//  Serial.println("end1");
//
//  //print(numerical_g_th2);
//  Serial.println("ana");
//  //print(analysis_grad[1]);
//  Serial.println("end2");
//
//  subtract(analysis_grad[0], numerical_g_th1);
//  double diff = 0;
//  for (i = 0; i < temp.n; i++)
//  {
//    for (j = 0; j < temp.h; j++)
//    {
//      diff +=
//      ((analysis_grad[0])->numbers[i][j])*((analysis_grad[0])->numbers[i][j]);
//    }
//  }
//  diff = sqrt(diff/(double)(temp.n*temp.h));
//  Serial.println("theta1 gradient error:");
//  Serial.println(diff);
//
//  subtract(analysis_grad[1], numerical_g_th2);
//  diff = 0;
//  for (int i = 0; i < temp.h; i++)
//  {
//    for (jnt j = 0; j < temp.o; j++)
//    {
//      diff +=
//      ((analysis_grad[1])->numbers[i][j])*((analysis_grad[1])->numbers[i][j]);
//    }
//  }
//  diff = sqrt(diff/(double)(temp.h*temp.o));
//  Serial.println("theta2 gradient error:");
//  Serial.println(diff);
//}

void multiply_sample_theta1()
{
  unsigned int i = 0, j = 0, k = 0;
  for (i = 0; i < NN_m; i++) 
  {
    for (j = 0; j < NN_h; j++)
    {
      a1[i][j] = 0;
      for (k = 0; k < NN_n; k++)
      {
        a1[i][j] += sample[i][k]*theta1[k][j];
      }
    }
  }
}

void multiply(double *matrix1, int m1, int n1, double *matrix2, int m2, int n2,
double *res)
{
  unsigned int i = 0, j = 0, k = 0;
  for (i = 0; i < m1; i++) 
  {
    for (j = 0; j < n2; j++)
    {
      *((res+i*n2)+j) = 0;
      for (k = 0; k < n1; k++)
      {
        *((res+i*n2)+j) += (*((matrix1+i*n1)+k))*(*((matrix2+k*n2)+j));
      }
    }
  }
}


void print_m(double *matrix, int m, int n)
{
  for (int i = 0; i < m; i++)
  {
    Serial.print("[");
    for (int j = 0; j < n; j++)
    {
      Serial.print(*((matrix+i*n) + j));
      Serial.print(" ,");
    }
    Serial.print("],");
    Serial.print("\n");
  }
}
