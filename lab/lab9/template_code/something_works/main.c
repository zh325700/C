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


FILE  * fp_data_out, * fp_text;

sbuffer_t  * buffer;
pthread_barrier_t sync_here;
int write_finish=0;        // to indicate if the write has finished

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


void *data_writer( ) {
	// this code runs in a thread
    FILE  * fp_sensor_data;
	int  presult,result;
    sensor_data_t * data;
    data = (sensor_data_t *)malloc(sizeof(sensor_data_t));
    fp_sensor_data=fopen ("sensor_data", "r");
    
    if(data==NULL){printf("data is null");pthread_exit(NULL);}
    
    while((fread(&data->id, sizeof(sensor_id_t), 1, fp_sensor_data)==1)
        &&(fread(&data->value, sizeof(sensor_value_t), 1, fp_sensor_data)==1)
        &&(fread(&data->ts, sizeof(sensor_ts_t), 1, fp_sensor_data)==1))
        {
//        presult = pthread_mutex_lock( &data_mutex );
//		pthread_err_handler( presult, "pthread_mutex_lock", __FILE__, __LINE__ );
        
        printf("data_write:%"PRIu16", %f, %ld\n",data->id,data->value,data->ts);
        result = sbuffer_insert( buffer,  data); //往buffer里面写东西
        if(result==SBUFFER_FAILURE){pthread_exit(NULL);}
        
//		presult = pthread_mutex_unlock( &data_mutex );
//		pthread_err_handler( presult, "pthread_mutex_unlock", __FILE__, __LINE__ );
        
        //pthread_barrier_wait( &sync_here );  
        usleep(10000);
		}
	
    free(data);
    fclose(fp_sensor_data);
    write_finish=1;
    printf("write finished\n");
	pthread_exit( NULL );
}


void *data_reader( void *id) {
	// this code runs in a thread
	int  presult,result;

    sensor_data_t  data_out;
    //data_out = (sensor_data_t *)malloc(sizeof(sensor_data_t));


    printf("you are now in read\n");
    while(1){
        result = sbuffer_remove( buffer,&data_out);

                    if(result==SBUFFER_FAILURE){pthread_exit(NULL);}               //退出线程了以后就不需要先unlock了
                    if(result==SBUFFER_NO_DATA&&write_finish==1){pthread_exit(NULL);}
                    if(result==SBUFFER_SUCCESS)
                        {
                    printf("data_read%d:,%"PRIu16", %f, %ld\n",*(int *)id,data_out.id,data_out.value,data_out.ts);
                    fwrite(&data_out.id,sizeof(uint16_t),1,fp_data_out);
                    fwrite(&data_out.value,sizeof(double),1,fp_data_out);
                    fwrite(&data_out.ts,sizeof(time_t),1,fp_data_out);
                    fprintf(fp_text,"%"PRIu16", %f, %ld\n",data_out.id,data_out.value,data_out.ts);
                    presult = pthread_barrier_wait( &sync_here );                //防止就是说， nodata 的时候也进入了barrier
                    usleep(5000);
                        }
                    if(result==SBUFFER_NO_DATA&&write_finish==0) usleep(1000);
            
        


    }     // while write not finished , we should wait for data if now buffer is empty
    
   // free(data_out);

	pthread_exit(NULL);
}

//void *data_reader( void *id) {
//    int result;
//    sensor_data_t data_out;
//    while(buffer!=NULL)
//    {
//        result=sbuffer_remove(buffer,&data_out);
//        pthread_err_handler( result, "pthread_mutex_lock", __FILE__, __LINE__ );
//        if(result== SBUFFER_SUCCESS)
//        {
//        printf("data_read%d:,%"PRIu16", %f, %ld\n",*(int *)id,data_out.id,data_out.value,data_out.ts);
//        fwrite(&data_out.id,sizeof(uint16_t),1,fp_data_out);
//        fwrite(&data_out.value,sizeof(double),1,fp_data_out);
//        fwrite(&data_out.ts,sizeof(time_t),1,fp_data_out);
//        fprintf(fp_text,"%"PRIu16", %f, %ld\n",data_out.id,data_out.value,data_out.ts);
//        pthread_barrier_wait(&sync_here);
//        usleep(10000);
//            }
//         else  if(result==SBUFFER_NO_DATA && write_finish==1) {pthread_exit(NULL);}
//            else {usleep(10000);}
//        }
//        pthread_exit(NULL);
//    }


int main(void) {
	int presult;
	pthread_t thread_A, thread_B,thread_C;
    int A_id,B_id;
    A_id=1;
    B_id=2;
    
    
    if(sbuffer_init(&buffer)!=SBUFFER_SUCCESS){exit(1);}
    
    fp_data_out=fopen ("sensor_data_out", "w");
    fp_text =fopen("data_text","w");	
//    presult = pthread_mutex_init(&data_mutex,NULL);
//    pthread_err_handler( presult, "pthread_initialization", __FILE__, __LINE__ );

	presult = pthread_barrier_init( &sync_here, NULL, 2 );
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
    
	presult = pthread_create( &thread_B, NULL, &data_writer, NULL );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
    
	presult = pthread_create( &thread_A, NULL, &data_reader, &A_id );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );
    
	presult = pthread_create( &thread_C, NULL, &data_reader, &B_id );
	pthread_err_handler( presult, "pthread_create", __FILE__, __LINE__ );	
      
	// important: don't forget to join, otherwise main thread exists and destroys the mutex
	presult= pthread_join(thread_B, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
    printf("thread B finnished\n");

	presult= pthread_join(thread_A, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
    printf("thread A finnished\n");

	presult= pthread_join(thread_C, NULL);
	pthread_err_handler( presult, "pthread_join", __FILE__, __LINE__ );
    printf("thread C finnished\n");

	presult = pthread_mutex_destroy( &data_mutex );
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
    
    presult = pthread_barrier_destroy( &sync_here );                     
	pthread_err_handler( presult, "pthread_mutex_destroy", __FILE__, __LINE__ );
    
    fclose(fp_data_out);
    fclose(fp_text);
    printf("main terminate\n");
	pthread_exit(NULL);          //把本身的thread退出
    return 0;

}
