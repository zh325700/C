#define _GNU_SOURCE 
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "sbuffer.h"
#include <pthread.h>
#include <sys/time.h>
#include "connmgr.h"

pthread_mutex_t buf_mutex;
pthread_barrier_t sync_here;     //这里嫁了
pthread_cond_t dbInit;
int presult;


void pthread_err_handler( int err_code, char *msg, char *file_name, char line_nr );

typedef struct sbuffer_node {
  struct sbuffer_node * next;
  sensor_data_t data;
} sbuffer_node_t;

struct sbuffer {
  sbuffer_node_t * head;
  sbuffer_node_t * tail;
};	


int sbuffer_init(sbuffer_t ** buffer)
{
  *buffer = malloc(sizeof(sbuffer_t));
  presult=pthread_mutex_init(&buf_mutex,NULL);           //这里改成静态初始化
  if (*buffer == NULL) return SBUFFER_FAILURE;
  presult = pthread_barrier_init( &sync_here, NULL, 2 );                  //这里加了initial 2 thread sync here
  pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
  presult = pthread_cond_init(&dbInit,NULL);
  pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
  (*buffer)->head = NULL;
  (*buffer)->tail = NULL;
  return SBUFFER_SUCCESS; 
}


int sbuffer_free(sbuffer_t ** buffer)
{
  sbuffer_node_t * dummy;
  presult = pthread_mutex_lock(&buf_mutex);
  pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
  if ((buffer==NULL) || (*buffer==NULL)) 
  {
    presult = pthread_mutex_unlock( &buf_mutex );
    pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
    return SBUFFER_FAILURE;
  }
   presult = pthread_mutex_unlock( &buf_mutex );
  pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
  while ( (*buffer)->head )
  {
    dummy = (*buffer)->head;
    (*buffer)->head = (*buffer)->head->next;
    free(dummy);
  }
  free(*buffer);
  *buffer = NULL;

  presult = pthread_mutex_destroy(&buf_mutex);
  pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
    presult = pthread_barrier_destroy( &sync_here );                                     //这里嫁了
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
  return SBUFFER_SUCCESS;		
}


int sbuffer_remove(sbuffer_t * buffer,sensor_data_t * data)
{
  sbuffer_node_t * dummy;
  int b_result=0;
  int c_result=0;
  struct timeval now;
  struct timespec outtime;

  if (buffer == NULL) {
  return SBUFFER_FAILURE;
  }
  
  presult = pthread_mutex_lock(&buf_mutex);
  pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
  
  if (buffer->head == NULL)
  { //这时候要等一些时间，看看有没有新的data进来，有的话返回success,不然NO_DATA   
  gettimeofday(&now,NULL);
  outtime.tv_sec = now.tv_sec+TIMEOUT;
  outtime.tv_nsec = now.tv_usec * 1000; 
  c_result = pthread_cond_timedwait(&dbInit,&buf_mutex,&outtime); // wait for new data add to buffer
  }
  if(c_result==ETIMEDOUT)    // if timeout return NO_DATA
      {
          printf("time out no data\n");
          presult = pthread_mutex_unlock(&buf_mutex);
          pthread_err_handler(presult,"pthread_mutex_unlock",__FILE__,__LINE__);
          return SBUFFER_NO_DATA;          
        }
  presult = pthread_mutex_unlock(&buf_mutex);
  pthread_err_handler(presult,"pthread_mutex_unlock",__FILE__,__LINE__);
  
  *data = buffer->head->data;
  dummy = buffer->head;
  
  b_result = pthread_barrier_wait( &sync_here );                //防止就是说， nodata 的时候也进入了barrier
  
  presult = pthread_mutex_lock(&buf_mutex);
  pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
  
if(b_result==0){

	if (buffer->head == buffer->tail) // buffer has only one node
	{
			buffer->head = buffer->tail = NULL; 
	}
	else  // buffer has many nodes empty
	{
		buffer->head = buffer->head->next;

	}
    free(dummy);
}
  presult = pthread_mutex_unlock(&buf_mutex);
  pthread_err_handler(presult,"pthread_mutex_unlock",__FILE__,__LINE__); 
  
  return SBUFFER_SUCCESS;
}


int sbuffer_insert(sbuffer_t * buffer, sensor_data_t * data)
{
  sbuffer_node_t * dummy;
  dummy = malloc(sizeof(sbuffer_node_t));  //后面记得free dummy
  if (buffer == NULL) {
    free(dummy);  
    return SBUFFER_FAILURE;}
  if (dummy == NULL)
 {
     return SBUFFER_FAILURE;
  }
    
  dummy->data = *data;
  dummy->next = NULL;
  
    presult = pthread_mutex_lock( &buf_mutex );
    pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
    presult = pthread_cond_signal(&dbInit);       // when insert success , send signal to read thread
    pthread_err_handler( presult, "pthread_cond_signal", __FILE__, __LINE__ );
  if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
  { 
    buffer->head = buffer->tail = dummy;
  }
  else if(buffer->head==buffer->tail)
  {
    buffer->tail->next = dummy;
    buffer->tail = buffer->tail->next;
  } 
  else // buffer not empty
  {
    buffer->tail->next = dummy;
    buffer->tail = buffer->tail->next;
  }
    presult = pthread_mutex_unlock( &buf_mutex );
    pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
  return SBUFFER_SUCCESS;

}




