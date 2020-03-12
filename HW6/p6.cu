#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <cuda.h>
#include <cuda_runtime.h>

/*
Name: Yiting Wang
M#: 01360917
Project: CSCI 6330 HW6
*/

# define Nthrds 100 // set the number of the thread

//set initial temperature for matrix
void Initial(double **M,int num_rows, int num_cols, double top,double left, double right, double bottom);

__global__ void calculate(double *dpreV, double *dnewV, double *ddouInfo, int *dintInfo)
{
  int num_rows = dintInfo[0];
  int num_cols = dintInfo[1];
  int step = dintInfo[2];
  int NumThrds = dintInfo[3];
  int rowsearch = dintInfo[4];
  int i,j;
  int tid = threadIdx.x; // get the gpu thread
 
  double eps = ddouInfo[0];
  double Imax_err;

  __shared__ double max_err;
  __shared__ double *err;
  
  err = (double *)malloc(NumThrds*sizeof(double));

  max_err = ddouInfo[1];
  //printf("max_err:%f\n",max_err);

  double loc_err = max_err;
  double temp_err;

  int start_row = (((tid)*rowsearch) > (0) ? ((tid)*rowsearch) : (0));
  int end_row = (((tid+1)*rowsearch + 1) < (num_rows - 1) ? ((tid+1)*rowsearch+1):(num_rows-1));

  if(tid == NumThrds-1)
  {
    end_row = num_rows - 1;
  }
  //printf("rowsearch:%d\n",rowsearch);
  //printf("start: %d end: %d tid: %d\n",start_row, end_row,tid);

  int index;
  while(max_err > eps)
  {
    loc_err = 0.0;
    //get the new value of matrix
    for(i=0;i<(end_row-start_row-1);i++)
    {
      for(j=1;j<num_cols-1;j++)
      {
        index = (start_row+i+1)*num_cols + j;
	dnewV[index] = (dpreV[index-1] + dpreV[index+1] + dpreV[index-num_cols] + dpreV[index+num_cols])/4.0;
	//printf("%f, %f, %f, %f, %f\n",preV[index-1],preV[index+1],preV[index-num_cols],preV[index+num_cols],dnewV[index]);
        temp_err = fabs(dnewV[index] - dpreV[index]);
	//printf("dnewV[%d] = %f, preV[%d] = %f, temp_err:%f, loc_err:%f\n",index,dnewV[index],index,preV[index],temp_err,loc_err);
        if(temp_err>loc_err)
        {
          loc_err = temp_err;
        }
      }
    }

    err[tid] = loc_err;
    //printf("loc_err:%f\n",loc_err);

    __syncthreads();

    //copy back to preV
    for(i=0;i<(end_row-start_row-1);i++)
    {
      for(j=1;j<num_cols-1;j++)
      {
        index = (start_row+1+i)*num_cols + j;
	//printf("index:%d\n",index);
        dpreV[index] = dnewV[index];
      }	
    }
    
    if(tid == NumThrds-1)
    {
 
      Imax_err = err[0];
      //printf("%f,err[0]=%f\n",Imax_err,err[0]);
      for(i=1;i<NumThrds;i++)
      {
        if(Imax_err<err[i])
	Imax_err = err[i];
	//printf("%f\n",Imax_err);
	//printf("%d,%f\n",i,err[i]);
      }
      max_err = Imax_err;
          
      if(step&(step-1))
      {
      }
      else
      {
        if(step>0)
        printf("%6d  %7lf\n",step, max_err);
      }
    }
    step = step+1;
    
    __syncthreads();
}
  if(tid == NumThrds-1)
  {
    printf("%6d  %7lf\n",step, max_err);
  }  	         
}

int main(int argc, char *argv[])
{
  int i;
  double top_temp,left_temp,right_temp,bottom_temp,eps;
  int num_rows, num_cols;
  double **preM; // matrix in host
  double **newM;
  
  double *dpreV; // vector in device
  double *dnewV;
  double *ddouInfo;
  int *dintInfo;
    
  double *preV; // vector in host
  double *newV;
  double *douInfo;
  int *intInfo;
  
  // read the initial number
  num_rows = atoi(argv[1]);
  num_cols = atoi(argv[2]);
  top_temp = atof(argv[3]);
  left_temp = atof(argv[4]);
  right_temp = atof(argv[5]);
  bottom_temp = atof(argv[6]);
  eps = atof(argv[7]);


  //matrix in  the host
  preM = (double **)malloc(num_rows*sizeof(double *));
  newM = (double **)malloc(num_rows*sizeof(double *));

  //malloc in the host
  preV = (double *)malloc(num_rows*num_cols*sizeof(double));
  newV = (double *)malloc(num_rows*num_cols*sizeof(double));
  douInfo = (double *)malloc(2*sizeof(double));
  intInfo = (int *)malloc(5*sizeof(int));
  
  // cuda malloc 
  cudaMalloc(&dpreV, sizeof(double)*num_rows*num_cols);
  cudaMalloc(&dnewV, sizeof(double)*num_rows*num_cols);
  cudaMalloc(&ddouInfo,sizeof(double)*2);
  cudaMalloc(&dintInfo,sizeof(int)*5);

  for(i=0;i<num_rows;i++)
  {
    preM[i] = &(preV[i*num_cols]);
    newM[i] = &(newV[i*num_cols]);
  }

  //set initial number for matrix
  Initial(preM,num_rows,num_cols,top_temp,left_temp,right_temp,bottom_temp);

  int rowsearch = ceil((num_rows - 2)*1.0/(Nthrds));

  for(i=0;i<num_cols;i++)
  {
    newM[0][i] = preM[0][i];
    newM[num_rows-1][i] = preM[num_rows-1][i];
  }

  for(i=0;i<num_rows;i++)
  {
    newM[i][0] = preM[i][0];
    newM[i][num_cols-1] = preM[i][num_cols-1];
  }

  //douInfo
  double max_err = 1000.0;
  douInfo[0] = eps;
  douInfo[1] = max_err;

  //intInfo
  int step = 0; 
  intInfo[0] = num_rows;
  intInfo[1] = num_cols;
  intInfo[2] = step;
  intInfo[3] = Nthrds;
  intInfo[4] = rowsearch; 

  // copy from host to device
  cudaMemcpy(ddouInfo,douInfo,2*sizeof(double),cudaMemcpyHostToDevice);
  cudaMemcpy(dintInfo,intInfo,5*sizeof(int),cudaMemcpyHostToDevice);
  cudaMemcpy(dpreV,preV,num_rows * num_cols * sizeof(double),cudaMemcpyHostToDevice);
  cudaMemcpy(dnewV,newV,num_rows * num_cols * sizeof(double),cudaMemcpyHostToDevice);

  calculate<<<1,Nthrds>>>(dpreV, dnewV, ddouInfo, dintInfo);

  // copy from device to host
  //cudaMemcpy(douInfo,ddouInfo,2*sizeof(double),cudaMemcpyDeviceToHost);
  //cudaMemcpy(intInfo,dintInfo,5*sizeof(int),cudaMemcpyDeviceToHost);
  
  free(preV);
  free(newV);
  free(douInfo);
  free(intInfo);

  cudaFree(dpreV);
  cudaFree(dnewV);
  cudaFree(ddouInfo);
  cudaFree(dintInfo);

  return 0;
}


void Initial(double **M, int num_rows, int num_cols, double top,double left, double right, double bottom)
{
  int i,j,count;
  double sum,average;

  for(i=0;i<num_cols;i++)
  {
    M[0][i] = top;
    M[num_rows-1][i] = bottom;
  }

  for(i=0;i<num_rows-1;i++)
  {
    M[i][0] = left;
    M[i][num_cols-1] = right;
  }
  
  //the sum of the boundary points
  sum = (top)*(num_rows-2) + bottom*(num_rows) + left*(num_cols-1) + right*(num_cols-1);

  //number of points in bound
  count = num_rows*2 + num_cols*2 -4;
  //average of the boundary points
  average = sum*1.0/(count*1.0);
  for (i=1;i<num_rows-1;i++)
  {
    for (j=1;j<num_cols-1;j++)
    {
      M[i][j] = average;
    }
  }
}
											      
