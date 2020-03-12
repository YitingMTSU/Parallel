#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>

double evalfunc(int nodeid, int *expandid1, int *expandid2);

static pthread_t* handles = 0; // thread handles
static int* nodes = 0; // node number
static int nodes_r = 0;
static int nodes_w = -1;

static int done = 0; // number of threads that finish the task
static int total = 0; // total number of threads
static int status = 0; // 0: normal, -1: abnormal, -2: searching-done
static int search_sol = 0;

pthread_mutex_t mutex;

// increase by 1 atomically
#define atomic_inc(value) __sync_fetch_and_add(&value, 1)

// decrease by 1 atomically
#define atomic_dec(value) __sync_fetch_and_sub(&value, 1)

// the main function to spearate the list
void* MainFunc(void* param);

// put two nodes to node tree and return an old one
int put_get_node(int node1, int node2);

// finalize and deinit all threads
void Deinitial();

// create threads
int Initial(int thrd_num, int sol);

int main(int argc, char *argv[])
{
  if(argc != 3)
    {
      printf("Usage: <T:thread-num> <N:searching-number-solution>\n");
      return -1;
    }

  int thrd_num = atoi(argv[1]);
  int sol = atoi(argv[2]);
	
  if(Initial(thrd_num, sol)<0)
    {
      Deinitial();
      return -1;
    }

  while(done<total)
    {
      usleep(1000000);
    }
  
  printf("Finish searching and totally %d nodes are tested.\n", nodes_r);

  Deinitial();
  return 0;
}

// create threads
int Initial(int thrd_num, int sol)
{
  int i, t1, t2;

  // create threads handle
  handles = (pthread_t*)malloc(sizeof(pthread_t)*thrd_num);
  if(!handles)
    {
      printf("WARMING: Memory error for pthread handles.\n");
      return -1;
    }

  search_sol = sol;

  // create task list
  nodes = (int*)malloc(sizeof(int)*10000000);
  if(!nodes)
    {
      printf("WARMING: Memory error for nodes queue.\n");
      return -2;
    }

  // first node from 0
  nodes[0] = 0;
  nodes_w = 0;

  pthread_mutex_init(&mutex, 0);

  // just for the first time's initial
  evalfunc(0, &t1, &t2);

  // create threads and make them run
  for(i=0; i<thrd_num; i++)
    {
      if(pthread_create(&handles[i], 0, MainFunc, 0))
	{
	  printf("ERROR: failed to create the %dth thread.\n", i);
	  status = -1; // make sure that create all threads
	  return -3;
	}
      else
	{
	  total++;
	}
    }
  return 0;
}

// finalize and deinit all threads
void Deinitial()
{
  if(handles)
    {
      free(handles);
      handles = 0;
    }

  if(nodes)
    {
      free(nodes);
      nodes = 0;
    }

  nodes_r = 0;
  nodes_w = -1;

  done = 0;
  total = 0;
  status = 0;
}

// put two nodes to task list and return an old one
int put_get_node(int node1, int node2)
{
  
  int r;
  pthread_mutex_lock(&mutex);
  if(node1 != -1)
    {
      nodes[++nodes_w] = node1;
    }
  if(node2 != -1)
    {
      nodes[++nodes_w] = node2;
    }

  r = nodes_r>nodes_w ? -1 : nodes[nodes_r++];
  pthread_mutex_unlock(&mutex);
  return r;
}

// main function separate the list
void* MainFunc(void* param)
{
  double rc;
  int max=0, current=0, e1 = -1, e2 = -1;

  // make threads automatically exit when they have finished
  pthread_detach(pthread_self());

  while(done<total)
    {
      if(max < current)
	{
	  max = current;
	}
      if(search_sol == 0)
	{
	  atomic_inc(done);
	  status = -2;
	  break;
	}

      current = put_get_node(e1, e2);
      if(current == -1)
	{
	  if(status<0 || evalfunc(max+1, &e1, &e2)==0.0 || search_sol == 0) // test if searching-done
	    {
	      atomic_inc(done);
	      status = -2;
	      break;
	    }
	  else
	    {
	      e1 = -1;
	      e2 = -1;
	      continue;
	    }
	}
      rc = evalfunc(current, &e1, &e2);
      if(rc == 0.0) // if done
	{
	  atomic_inc(done);
	  status = -2;
	  break;
	}
      else if(rc > 1.0) // if status
	{
	  atomic_dec(search_sol);
	  printf("Solution: NodeID=%5d, NodeValue=%.1f\n", current, rc);
	}
    }
  return 0;
}
