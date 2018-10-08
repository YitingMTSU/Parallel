#include<stdio.h>
#include "mpi.h"
#include<math.h>
#include<stdlib.h>

/*
Name: Yiting Wang
M#: 01360917
Project: CSCI 6330 HW2
*/

//set initial temperature for matrix
double Initial(double **M,int num_rows, int num_cols, double top,double left, double right, double bottom);


int main(int argc, char *argv[])
{
  int i,j,myrank,numranks;
  double top_temp,left_temp,right_temp,bottom_temp;
  int num_rows, num_cols;
  double **pre;
  double **new;
  double **temp;
  double *preM;
  double *newM;
  double *tempM;
  double max_err;
  double eps;
  int step = 0; // the num of the steps
  MPI_Status status;
  int tag = 123;

  //read initial numbers
  num_rows = atoi(argv[1]);
  num_cols = atoi(argv[2]);
  top_temp = atof(argv[3]);
  left_temp = atof(argv[4]);
  right_temp = atof(argv[5]);
  bottom_temp = atof(argv[6]);
  eps = atof(argv[7]);

  double a[7];


  //declare the matrix
  pre = (double **)malloc(num_rows*sizeof(double *));
  new = (double **)malloc(num_rows*sizeof(double *));
  temp = (double **)malloc(num_rows*sizeof(double *));
  preM = (double*)malloc(num_rows*num_cols*sizeof(double));
  newM = (double*)malloc(num_rows*num_cols*sizeof(double));
  tempM = (double*)malloc(num_rows*num_cols*sizeof(double));

  double *p = &preM[0];
  double *q = &newM[0];
  double *r = &tempM[0];
  for(i=0;i<num_rows;i++)
    {
      /*pre[i] = p;
      p += num_cols*sizeof(double);
      new[i] = q;
      q += num_cols*sizeof(double);
      temp[i] = r;
      r += num_cols*sizeof(double);*/
      pre[i] = &(preM[num_cols*i]);
      new[i] = &(newM[num_cols*i]);
      temp[i] = &(tempM[num_cols*i]);
    }
    
  MPI_Init(&argc,&argv);
  MPI_Comm_size(MPI_COMM_WORLD,&numranks);
  MPI_Comm_rank(MPI_COMM_WORLD,&myrank);

  //printf("22,%d\n",myrank);

  if(myrank == 0)
    {      
      //set initial number for matrix
      Initial(temp,num_rows,num_cols,top_temp,left_temp,right_temp,bottom_temp);
      for (i=0; i<num_rows;i++)
	{
	  printf("(%d): ",i);
	  for (j=0; j<num_cols; j++)
	    {
	      printf("%lf ",temp[i][j]);
	    }
	  printf("\n");
	}
    }

  MPI_Barrier(MPI_COMM_WORLD);
    
  max_err = 1000.0;
  
   
  //while(max_err > eps)
    {
       if(myrank == 0)
	{
	  // make a copy from newM to preM
	  for(i=0;i<num_rows;i++)
	    {	      
	      for(j=0;j<num_cols;j++)
		{
		  new[i][j] = temp[i][j];
		  pre[i][j] = new[i][j];
		}
	    }
	  //printf("33,%d\n",myrank);
	  for (i=0;i<7;i++)
	    {
	      a[i]=0.5/i;
	    }

	  
	  
	  int rowsearch; // how many rows contained in each process
       
	   for(i=1;i<numranks;i++)
	    {
	      //printf("%d\n",i);
	      int id = i-1;
	      rowsearch = ceil((num_rows - 2)/(numranks-1));
	      int start_row = ((id*rowsearch) > (0)? (id*rowsearch):(0));
	      int end_row = (((id+1)*rowsearch+1) < (num_rows-1)? ((id+1)*rowsearch+1):(num_rows-1));

	      //printf("rowsearch : %d(%d)\n",rowsearch,myrank);
	      //printf("start_row: : %d(%d)\n",start_row,myrank);
	      //printf("end_row: : %d(%d)\n",end_row,myrank);
	      printf("22 %d\n",id+1);

	      // send the matrix information to other process
	      MPI_Send(a,7,MPI_DOUBLE,id+1,tag,MPI_COMM_WORLD);

	      printf("33 %d\n",id+1);
	      // receive the matrix information from other process
	      //MPI_Recv(&new[start_row+1][0],(end_row-start_row-2)*num_cols,MPI_DOUBLE,id,tag,MPI_COMM_WORLD,&status);
	    }
	  
	  //compare the maxmimum error in preM and newM
	  /*	   
	  double local_err = 0.0;
	  double temp_err;
	  for(i=0;i<num_rows;i++)
	    {
	      for(j=0;j<num_cols;j++)
		{
		  temp_err = fabs(new[i][j] - pre[i][j]);
		  if(local_err<temp_err)
		    {
		      local_err = temp_err;
		    }
		}
	    }
	  max_err = local_err;
	  step = step + 1;


	  // print information for the iterrations
	  int num,numc,numf;

	  num = log(step)/log(2);
	  numc = ceil(num);
	  numf = floor(num);
	  if(num == numc && num == numf && num>-1)
	    {
	      printf("%d  %lf\n",step, max_err);
	    }
	  
	    MPI_Bcast(&max_err,1,MPI_DOUBLE,0,MPI_COMM_WORLD);*/
	}
       else
	 {
	  //printf("44,%d\n",myrank);
	  
	  
	  double average;
	  int id = myrank-1;
	  int rowsearch = ceil((num_rows - 2)/(numranks-1));
	  int start_row = ((id*rowsearch) > (0)? (id*rowsearch):(0));
	  int end_row = (((id+1)*rowsearch+1) < (num_rows-1)? ((id+1)*rowsearch+1):(num_rows-1));

	  //printf("rowsearch : %d(%d)\n",rowsearch,myrank);
	  //printf("start_row: : %d(%d)\n",start_row,myrank);
	  //printf("end_row: : %d(%d)\n",end_row,myrank);
	  printf("44 %d\n",tag);

	  // Receive the information from the original process
	  MPI_Recv(a,7,MPI_DOUBLE,0,tag,MPI_COMM_WORLD,&status);
	  printf("55 %d\n",id+1);
	  for (i=0; i<num_rows;i++)
	    {
	      printf("(%d): ",i);
	      for (j=0; j<num_cols; j++)
		{
		  printf("%lf ",temp[i][j]);
		}
	      printf("\n");
	    }
	  

	  // calculate the new temperature
	  /* for (i=start_row+1;i<end_row;i++)
	    {
	      for (j=1;j<num_cols-1;j++)
		{
		  average = (temp[i+1][j] + temp[i-1][j] + temp[i][j+1] +\
			     + temp[i][j-1])/4.0;
		  temp[i][j] = average;
		}
	    }
	  for (i=0; i<num_rows;i++)
	    {
	      printf("(%d): ",i);
	      for (j=0; j<num_cols; j++)
		{
		  printf("%lf ",temp[i][j]);
		}
	      printf("\n");
	    }
	  
	  */
	  //send the matrix back to the process 0
	  //MPI_Send(&new[start_row+1][0],(end_row-start_row-2)*num_cols,MPI_DOUBLE,0,tag,MPI_COMM_WORLD);
	  //MPI_Barrier(MPI_COMM_WORLD);
	 
	  }
	}	  

  

  MPI_Finalize();
  return 0;
  //free(pre);free(new);free(temp);
  //free(preM);free(newM);free(tempM);
}



double Initial(double **M, int num_rows, int num_cols, double top,double left, double right, double bottom)
{
  int i,j,count;
  double sum,average;

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

