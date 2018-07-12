#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "sbuffer.h"
#include <pthread.h>

#ifndef NUM_READERS
   #define NUM_READERS 2         // user can define how many thread are there 
#endif 

pthread_mutex_t buf_mutex;
int presult;


void pthread_err_handler( int err_code, char *msg, char *file_name, char line_nr );

typedef struct sbuffer_node {
  struct sbuffer_node * next;
  sensor_data_t data;
  int count_thread;
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
  return SBUFFER_SUCCESS;		
}


int sbuffer_remove(sbuffer_t * buffer,sensor_data_t * data)
{
  sbuffer_node_t * dummy;
  presult = pthread_mutex_lock(&buf_mutex);
	pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
  if (buffer == NULL) {
  presult = pthread_mutex_unlock(&buf_mutex);
  pthread_err_handler(presult,"pthread_mutex_unlock",__FILE__,__LINE__); 
  return SBUFFER_FAILURE;
  }
  if (buffer->head == NULL) {
  presult = pthread_mutex_unlock(&buf_mutex);
  pthread_err_handler(presult,"pthread_mutex_unlock",__FILE__,__LINE__); 
  return SBUFFER_NO_DATA;
  }
  
  *data = buffer->head->data;
  dummy = buffer->head;
   
  if(buffer->head->count_thread == NUM_READERS-1){
	if (buffer->head == buffer->tail) // buffer has only one node
	{
			buffer->head = buffer->tail = NULL; 
	}
	else  // buffer has many nodes empty
	{
		buffer->head = buffer->head->next;
		buffer->head->count_thread =0;

	}
  }
  else {
	  dummy->count_thread++;
//	  buffer->head = dummy;
  }
  
  presult = pthread_mutex_unlock(&buf_mutex);
  pthread_err_handler(presult,"pthread_mutex_unlock",__FILE__,__LINE__); 
  
  return SBUFFER_SUCCESS;
}


int sbuffer_insert(sbuffer_t * buffer, sensor_data_t * data)
{
  sbuffer_node_t * dummy;
  dummy = malloc(sizeof(sbuffer_node_t));  
    presult = pthread_mutex_lock( &buf_mutex );
    pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
  if (buffer == NULL) {    presult = pthread_mutex_unlock( &buf_mutex );
    pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
    return SBUFFER_FAILURE;}

  if (dummy == NULL) {    presult = pthread_mutex_unlock( &buf_mutex );
    pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );return SBUFFER_FAILURE;}
  dummy->data = *data;
  dummy->next = NULL;

  if (buffer->tail == NULL) // buffer empty (buffer->head should also be NULL
  {
    buffer->head = buffer->tail = dummy;
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



