#define _GNU_SOURCE 
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "time.h"
#include <errno.h>
#include "config.h"
#include "sbuffer.h"
#include <inttypes.h>
#include <string.h>
#include "datamgr.h"
#include "connmgr.h"
#include "sensor_db.h"
#include "errmacros.h"
#include <sys/types.h>
#include "fifo_globle.h"
#include <sys/stat.h>

#define MAX 80
#define LOG_NAME "gateway.log"
FILE  * fp_data_out, * fp_text, *fifo_fp;

sbuffer_t  * buffer;

DBCONN *con;

// mutex
pthread_mutex_t data_mutex= PTHREAD_MUTEX_INITIALIZER;//PTHREAD_ERRORCHECK_MUTEX_INITIALIZER;

void pthread_err_handler( int err_code, char *msg, char *file_name, char line_nr )
{
	if ( 0 != err_code )
	{
		fprintf( stderr, "\n%s failed with error code %d in file %s at line %d\n", msg, err_code, file_name, line_nr );
		//errno = err_code;
		//perror("Error message: ");
	}
}



void *tcp_con()
{
    connmgr_listen(5678,&buffer);
    usleep(20000);
    return NULL;
}

void *data_mgr()
{
FILE *fp_sensor_map;
fp_sensor_map=fopen ("room_sensor.map", "r");
datamgr_parse_sensor_data(fp_sensor_map,& buffer);
usleep(10000);
return NULL;
}

void *sensor_db()
{

    con=init_connection(1);
    storagemgr_parse_sensor_data(con, & buffer);    //程序会一直在这个函数里面，直到退出
    usleep(10000);
    disconnect(con);              //after all thread exit, then you disconnect
    return NULL;
}

void final_message(void) 
{
  pid_t pid = getpid();
  printf("Process %d is now exiting ...\n", pid);
}

void run_child (void)
{
    int result;
    char *str_result;
    double sn_count=0;                //sequence number counter
    FILE *log_fp,*log_text_fp;
    char recv_buf[MAX];
    time_t log_ts;
    result = mkfifo(FIFO_NAME, 0666);       // create fifo
    CHECK_MKFIFO(result);
    
    log_fp = fopen(FIFO_NAME, "r");             //open fifo
    FILE_OPEN_ERROR(log_fp);
    
    log_text_fp = fopen(LOG_NAME,"w");
    FILE_OPEN_ERROR(log_text_fp);
   // this terminates the child process
   printf("child process\n");
   
  do 
  {
    str_result = fgets(recv_buf, MAX, log_fp);
    if ( str_result != NULL )
    { 
      time(&log_ts);
      fprintf(log_text_fp,"%f %ld %s\n",sn_count,log_ts, recv_buf); 
    }
    sn_count++;
  } while ( str_result != NULL ); 
  fclose(log_fp);
  fclose(log_text_fp);
}

int write_to_fifo(char *str)
{
    if ( fputs( str, fifo_fp ) == EOF ) //fputs函数的功能是向指定的文件写入一个字符串，
    {
      fprintf( stderr, "Error writing data to fifo\n");
      return -1;
    } 
      return 0;
}

int main(void) {
	int presult,result;
	pthread_t thread_mgr, thread_con,thread_db;

    pid_t my_pid, child_pid;
 
      
    atexit( final_message );             //when exit the process, call the function final_message
    my_pid = getpid();                   // get the process id 
    printf("Parent process (pid = %d) is started ...\n", my_pid);
      
    child_pid = fork();
    SYSCALL_ERROR(child_pid);

    if ( child_pid == 0  )
    {  
        run_child();
    }
    else
    {
    if(sbuffer_init(&buffer)!=SBUFFER_SUCCESS){exit(1);}

        
    result = mkfifo(FIFO_NAME, 0666);       // create fifo
    CHECK_MKFIFO(result);
    
    fifo_fp = fopen(FIFO_NAME, "w");             //open fifo
    FILE_OPEN_ERROR(fifo_fp);
        
	presult = pthread_create( &thread_con, NULL, &tcp_con, NULL );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
    
	presult = pthread_create( &thread_mgr, NULL, &data_mgr, NULL );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
    
	presult = pthread_create( &thread_db, NULL, &sensor_db, NULL );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );	
      
	// important: don't forget to join, otherwise main thread exists and destroys the mutex
	presult= pthread_join(thread_con, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
    printf("thread B finnished\n");

	presult= pthread_join(thread_mgr, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
    printf("thread A finnished\n");

	presult= pthread_join(thread_db, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
    printf("thread C finnished\n");

	presult = pthread_mutex_destroy( &data_mutex );
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
    
    presult = sbuffer_free(&buffer);
    pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );

    printf("main terminate\n");

    sbuffer_free(&buffer);        //free the buffer
    result = fclose( fifo_fp );         //close the fifo 
    FILE_CLOSE_ERROR(result);
    printf("parent process\n");
	pthread_exit(NULL);          //把本身的thread退出
    exit(EXIT_SUCCESS);
    }
    
}

//void *data_writer( ) {
//	// this code runs in a thread
//    FILE  * fp_sensor_data;
//	int  presult,result;
//    sensor_data_t * data;
//    data = (sensor_data_t *)malloc(sizeof(sensor_data_t));
//    fp_sensor_data=fopen ("sensor_data", "r");
//    
//    if(data==NULL){printf("data is null");pthread_exit(NULL);}
//    
//    while((fread(&data->id, sizeof(sensor_id_t), 1, fp_sensor_data)==1)
//        &&(fread(&data->value, sizeof(sensor_value_t), 1, fp_sensor_data)==1)
//        &&(fread(&data->ts, sizeof(sensor_ts_t), 1, fp_sensor_data)==1))
//        {
//        
//        printf("data_write:%"PRIu16", %f, %ld\n",data->id,data->value,data->ts);
//        result = sbuffer_insert( buffer,  data); //往buffer里面写东西
//        if(result==SBUFFER_FAILURE){pthread_exit(NULL);}
// 
//        usleep(10000);
//		}
//	
//    free(data);
//    fclose(fp_sensor_data);
//    write_finish=1;
//    printf("write finished\n");
//	pthread_exit( NULL );
//}
//
//
//void *data_reader( void *id) {
//	// this code runs in a thread
//	int  presult,result;
//
//    sensor_data_t  data_out;
//
//    printf("you are now in read\n");
//    while(1){
//        result = sbuffer_remove( buffer,&data_out);     // remove itself is already thread safe.
//
//                    if(result==SBUFFER_FAILURE){pthread_exit(NULL);}               //退出线程了以后就不需要先unlock了
//                    if(result==SBUFFER_NO_DATA&&write_finish==1){pthread_exit(NULL);}
//                    if(result==SBUFFER_SUCCESS)
//                        {
//                    printf("data_read%d:,%"PRIu16", %f, %ld\n",*(int *)id,data_out.id,data_out.value,data_out.ts);
//                    fwrite(&data_out.id,sizeof(uint16_t),1,fp_data_out);
//                    fwrite(&data_out.value,sizeof(double),1,fp_data_out);
//                    fwrite(&data_out.ts,sizeof(time_t),1,fp_data_out);
//                    fprintf(fp_text,"%"PRIu16", %f, %ld\n",data_out.id,data_out.value,data_out.ts);
////                    presult = pthread_barrier_wait( &sync_here );                //防止就是说， nodata 的时候也进入了barrier
//                    usleep(5000);
//                        }
//                    if(result==SBUFFER_NO_DATA&&write_finish==0) usleep(1000);
//            
//        
//
//
//    }     // while write not finished , we should wait for data if now buffer is empty
//    
//
//	pthread_exit(NULL);
//}