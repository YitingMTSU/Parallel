#include "p1.h"

/*
Name: Yiting Wang
M#: 01360917
Project: CSCI 6330 HW1
*/

int num_rows, num_cols;

//set initial temperature for matrix
double Initial(double M[num_rows][num_cols],int num_rows, int num_cols, double top,double left, double right, double bottom);

//print the information for matrix
void printM(double M[num_rows][num_cols], int num_rows, int num_cols);

//iteration for the final answer
void Iteration(double preM[num_rows][num_cols], double nowM[num_rows][num_cols], int num_rows, int num_cols, double eps);

int main(int argc, char *argv[])
{
  
  double top_temp,left_temp,right_temp,bottom_temp;
  double eps;

  //read initial numbers
  num_rows = atoi(argv[1]);
  num_cols = atoi(argv[2]);
  top_temp = atof(argv[3]);
  left_temp = atof(argv[4]);
  right_temp = atof(argv[5]);
  bottom_temp = atof(argv[6]);
  eps = atof(argv[7]);


  printf("Here\n");
  
  //set the matrix
  double preM[num_rows][num_cols];
  printf("Here\n");
  
  double nowM[num_rows][num_cols];
  printf("Here\n");
  //set initial number for matrix
  Initial(preM,num_rows,num_cols,top_temp,left_temp,right_temp,bottom_temp);
  //printM(preM,num_rows,num_cols);

  //iteration
  Iteration(preM,nowM,num_rows,num_cols,eps);

  return 0;
}


double Initial(double M[num_rows][num_cols], int num_rows, int num_cols, double top, double left, double right, double bottom)
{
  int i,j,count;
  double sum,average;
  printf("Here\n");
  for (i=0;i<num_cols;i++)
    {
      M[0][i] = top;
      M[num_rows-1][i] = bottom;
    }
  for (i=0;i<num_rows-1;i++)
    {
      M[i][0] = left;
      M[i][num_cols-1] = right;
    }
  //the sum of the boundary points
  sum = (top)*(num_rows-2) + bottom*(num_rows) + left*(num_cols-1) + right*(num_cols-1);
  //number of points in bound
  count = num_rows*2 + num_cols*2 -4;
  //average of the boundary points
  average = sum/count;
  for (i=1;i<num_rows-1;i++)
    {
      for (j=1;j<num_cols-1;j++)
	{
	  M[i][j] = average;
	}
    }
}
      


void printM(double M[num_rows][num_cols], int num_rows, int num_cols)
{
  int i,j;
  for (i=0; i<num_rows;i++)
    {
      printf("(%d): ",i);
      for (j=0; j<num_cols; j++)
	{
	  printf("%lf ",M[i][j]);
	}
      printf("\n");
    }
}


void Iteration(double preM[num_rows][num_cols], double nowM[num_rows][num_cols], int num_rows, int num_cols, double eps)
{
  int i,j,step;
  double average;
  double num,numc,numf;
  double diffM,diffe,diffetemp;// diffM means the difference between matrix, diffe means the difference between element

  diffM = 1.0;
  step = -1;
  //printf("eps:%lf\n",eps);
  while (diffM > eps)
    {
      diffe = 0.0;
      printM(preM,num_rows,num_cols);
      
      for (i=1;i<num_rows-1;i++)
	{
	  for (j=1;j<num_cols-1;j++)
	    {
	      average = (preM[i+1][j] + preM[i-1][j] + preM[i][j+1] + \
			 + preM[i][j-1])/4.0;
	      diffetemp = fabs(average - preM[i][j]);
	      if (diffetemp > diffe)
		{
		  diffe = diffetemp;
		}
	      nowM[i][j] = average;
	    }
	}
      for (i=1;i<num_rows-1;i++)
	{
	  for (j=1;j<num_cols-1;j++)
	    {
	      preM[i][j] = nowM[i][j];
	    }
	}
      printM(nowM,num_rows,num_cols);
      diffM = diffe;
      step = step + 1;
      num = log(step)/log(2);
      numc = ceil(num);
      numf = floor(num);
      if(num == numc && num == numf && num>-1)
	{
	  printf("%6d  %7lf\n",step, diffM);
	}

    }
  printf("\n");
  printf("%6d  %7lf\n",step+1,diffM);

}
