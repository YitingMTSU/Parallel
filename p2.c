#include "p2.h"

/*
Name: Yiting Wang
M#: 01360917
Project: CSCI 6330 HW2
*/

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
int num_rows, num_cols;
double **preM;
double **newM;
double max_err;
double *thrd_err;
double eps;
int num_thread;
int step = -1;
pthread_barrier_t barr;

//set initial temperature for matrix
double Initial(double top,double left, double right, double bottom);

//print the information for matrix
void printM();

//iteration for the final answer
void *Iteration(void * arg);

int main(int argc, char *argv[])
{
  
  double top_temp,left_temp,right_temp,bottom_temp;
  int i;

  //read initial numbers
  num_rows = atoi(argv[1]);
  num_cols = atoi(argv[2]);
  top_temp = atof(argv[3]);
  left_temp = atof(argv[4]);
  right_temp = atof(argv[5]);
  bottom_temp = atof(argv[6]);
  eps = atof(argv[7]);
  num_thread = atoi(argv[8]);


  pthread_t id[num_thread];
  int d[num_thread];
  //declare the matrix
  preM = (double**)malloc(num_rows*sizeof(double *));
  newM = (double**)malloc(num_rows*sizeof(double *));
  for(i=0;i<num_rows;i++)
  {
    preM[i] = (double*)malloc(num_cols*sizeof(double));
    newM[i] = (double*)malloc(num_cols*sizeof(double));
  }
  
 
  
  //set initial number for matrix
  Initial(top_temp,left_temp,right_temp,bottom_temp);
  //printM();

  //declare the error
  thrd_err = (double*)malloc(num_thread*sizeof(double));

  max_err = 1000;

  pthread_barrier_init(&barr,NULL,num_thread);
  //create thread
  for(i=0;i<num_thread;i++)
    {
      d[i] = i;
      pthread_create(&id[i],NULL,Iteration,(void *)&d[i]);
    }
  for(i=0;i<num_thread;i++)
    {
      pthread_join(id[i],NULL);
    }
  printf("\n");
  printf("%6d  %7lf\n",step,max_err);

  

  return 0;
}

double Initial(double top,double left, double right, double bottom)
{
  int i,j,count;
  double sum,average;
 
  //for(i=0;i<num_rows;i++)
  //  {
  //    for(j=0;j<num_cols;j++)
  //    {
  //      preM[i][j] = 0.0;
  //    }
  // }
  
  
  for (i=0;i<num_cols;i++)
    {
      preM[0][i] = top;
      preM[num_rows-1][i] = bottom;
    }
  for (i=0;i<num_rows-1;i++)
    {
      preM[i][0] = left;
      preM[i][num_cols-1] = right;
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
	  preM[i][j] = average;
	}
    }
}
      


void printM()
{
  int i,j;
  for (i=0; i<num_rows;i++)
    {
      printf("(%d): ",i);
      for (j=0; j<num_cols; j++)
	{
	  printf("%lf ",preM[i][j]);
	}
      printf("\n");
    }
}


void *Iteration(void *arg)
{
  int i,j,k;
  int id = *((int *)arg);
  double average;
  double num,numc,numf;
  double diffetemp;// diffM means the difference between matrix, diffe means the difference between element

  int rowseach; //how many rows contained in each pthread
  rowseach = ceil((num_rows - 2)/num_thread);
  int start_row = ((id*rowseach) > (0)? (id*rowseach):(0));
  int end_row = (((id+1)*rowseach+1) < (num_rows-1)? ((id+1)*rowseach+1):(num_rows-1));

    
  //printf("eps:%lf\n",eps);
  while (max_err > eps)
   {
  //for(k=0;k<389;k++){
      pthread_barrier_wait(&barr);
      thrd_err[id] = 0.0;
      pthread_mutex_lock(&lock);
      max_err = 0.0;
      pthread_mutex_unlock(&lock);
      for (i=start_row+1;i<end_row;i++)
	{
	  for (j=1;j<num_cols-1;j++)
	    {
	      average = (preM[i+1][j] + preM[i-1][j] + preM[i][j+1] +	\
			 + preM[i][j-1])/4.0;
	      diffetemp = fabs(average - preM[i][j]);
	      if (diffetemp > thrd_err[id])
		{
		  thrd_err[id] = diffetemp;
		}
	      newM[i][j] = average;
	    }
	}
      pthread_barrier_wait(&barr);

      pthread_mutex_lock(&lock);
      if(thrd_err[id] > max_err)
	{
	  max_err = thrd_err[id];
	}
      pthread_mutex_unlock(&lock);
      for (i=start_row+1;i<end_row;i++)
	{
	  for (j=1;j<num_cols-1;j++)
	    {
	      preM[i][j] = newM[i][j];
	    }
	}
      //pthread_barrier_wait(&barr);
      //  printM(preM,num_rows,num_cols);
      if(id == 1)
	{
	  step = step + 1;
          num = log(step)/log(2);
          numc = ceil(num);
          numf = floor(num);
          if(num == numc && num == numf && num>-1)
	   {
	     printf("%6d  %7lf\n",step, max_err);
	   }
	}
      //pthread_barrier_wait(&barr);
      //printf("%d,%lf\n",id,max_err);
  }
  // printf("down  %d,%lf\n",id,max_err);    //}
  

}

