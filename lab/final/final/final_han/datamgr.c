#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "datamgr.h"
#include "lib/dplist.h"
#include "time.h"
#include <inttypes.h>
#include "fifo_globle.h"

dplist_t * list_mgr = NULL;
char str_to_fifo[100];

int dplist_errno;

void* element_copy_mgr(void * element);
void element_free_mgr(void ** element);
int element_compare_mgr(void * x, void * y);
 
sensor_element_t * find_sensor_element_by_id(sensor_id_t sensor_id);
 /*
 *  This method holds the core functionality of your datamgr. It takes in 2 file pointers to the sensor files and parses them. 
 *  When the method finishes all data should be in the internal pointer list and all log messages should be printed to stderr.
 */
 
 void datamgr_parse_sensor_data(FILE * fp_sensor_map, sbuffer_t ** buffer)
{
    
    sensor_data_t *data;
    sensor_element_t *sensor_element,*dummy_element;
    list_mgr = dpl_create(element_copy_mgr, element_free_mgr, element_compare_mgr);     //create new list
    int i,result;           //result keep track on if the fread success
///    int fp;
    sensor_element = (sensor_element_t *) malloc(sizeof(sensor_element_t));
    data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    
    sensor_element->sensor_id = 0;
    sensor_element->room_id = 0;
    sensor_element->running_avg =0;
    sensor_element->index=0;
    sensor_element->flag=false;
    for(i=0;i<RUN_AVG_LENGTH;i++){sensor_element->last_sensor_data[i]=0;} // clear the sensor_element content   
    while (fscanf(fp_sensor_map,"%" SCNu16 " %" SCNu16 "\n", &sensor_element->room_id, &sensor_element->sensor_id) != EOF) { //room id and sensor id are all unsigned short int.
        dpl_insert_sorted(list_mgr, (void*)sensor_element, true);                                             // insert read sensor id and room id into List and make a deep copy
    }                                                                                             //SCNu16 make it restric 16 bits, not changing with different os
    
    
    while(*buffer!=NULL)
    {
        result = sbuffer_remove(* buffer,data);
        if(result==SBUFFER_FAILURE){printf("the buffer is NULL");free(data);break;}
        else if(result==SBUFFER_NO_DATA){printf("the buffer have no data\n");free(data);break;}
        else if(result==SBUFFER_SUCCESS)
        {
        dummy_element = find_sensor_element_by_id(data->id);         //get the element with certain sensor_id
        if (dummy_element != NULL) 
            {
                printf("data_read_mgr:,%"PRIu16", %f, %ld\n",data->id,data->value,data->ts);
                dummy_element->last_sensor_data[dummy_element->index] = data->value; // if the array is full then start overwrite the first one
                if(dummy_element->index<RUN_AVG_LENGTH-1) {dummy_element->index=(dummy_element->index)+1;}
                else {dummy_element->flag=true;dummy_element->index=0;}                 //if index = RUN_AVG_LENGTH it means the array is full
                dummy_element->last_modified = data->ts;
            
                if(dummy_element->flag==true)
                {
                    
                    dummy_element->running_avg = 0;          //reset the avg before calculate
                    for(i=0;i<RUN_AVG_LENGTH;i++)
                    {
                        dummy_element->running_avg+=dummy_element->last_sensor_data[i];
                    }
                    dummy_element->running_avg=(dummy_element->running_avg)/RUN_AVG_LENGTH;
                    
                    if ((dummy_element->running_avg) > SET_MAX_TEMP) 
                    {
                        fprintf(stderr, "The sensor node with %" PRIu16 " reports it's too hot, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);    //print uint16_t use %" PRIu16 "
                        snprintf(str_to_fifo,99,"the sensor node with %" PRIu16 " reports it's too hot, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);
                        write_to_fifo(str_to_fifo);
                        
                    }
                    else if ((dummy_element->running_avg) < SET_MIN_TEMP) 
                    {
                        fprintf(stderr, "The sensor node with %" PRIu16 " reports it's too cold, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);    //print uint16_t use %" PRIu16 "
                        snprintf(str_to_fifo,99,"the sensor node with %" PRIu16 " reports it's too hot, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);
                        write_to_fifo(str_to_fifo);
                        
                    }
                }  //14.01 27.96
                else if(dummy_element->flag==false)
                {
                    dummy_element->running_avg = 0;          //reset the avg before calculate
                    for(i=0;i<dummy_element->index;i++)
                    {
                        dummy_element->running_avg+=dummy_element->last_sensor_data[i];
                    }
                    dummy_element->running_avg =(dummy_element->running_avg)/(dummy_element->index);
                    
                    if ((dummy_element->running_avg) > SET_MAX_TEMP) 
                    {
                        fprintf(stderr, "The sensor node with %" PRIu16 " reports it's too hot, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);    //print uint16_t use %" PRIu16 "
                        snprintf(str_to_fifo,99,"the sensor node with %" PRIu16 " reports it's too hot, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);
                        write_to_fifo(str_to_fifo);    
                    }
                    else if ((dummy_element->running_avg) < SET_MIN_TEMP) 
                    {
                        fprintf(stderr, "The sensor node with %" PRIu16 " reports it's too cold, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);    //print uint16_t use %" PRIu16 "
                        snprintf(str_to_fifo,99,"the sensor node with %" PRIu16 " reports it's too hot, running avg temperature =%f\n", dummy_element->sensor_id,dummy_element->running_avg);
                        write_to_fifo(str_to_fifo);    
                    }
                }
                                
            }
            else
                {
                    fprintf(stderr, "Receive sensor with invalid sensor node ID: %" PRIu16 " \n", data->id);    //print uint16_t use %" PRIu16 "
                    snprintf(str_to_fifo,99,"Receive sensor with invalid sensor node ID: %" PRIu16 " \n", data->id);
                    write_to_fifo(str_to_fifo);
                }
            free(data);
        }
    }
    
    free(sensor_element);

    
}
 
 
void datamgr_parse_sensor_files(FILE * fp_sensor_map, FILE * fp_sensor_data)
{
    
    sensor_data_t *data;
    sensor_element_t *sensor_element,*dummy_element;
    list_mgr = dpl_create(element_copy_mgr, element_free_mgr, element_compare_mgr);     //create new list
    int i;           //result keep track on if the fread success
///    int fp;
    sensor_element = (sensor_element_t *) malloc(sizeof(sensor_element_t));
    data = (sensor_data_t *) malloc(sizeof(sensor_data_t));
    
    sensor_element->sensor_id = 0;
    sensor_element->room_id = 0;
    sensor_element->running_avg =0;
    sensor_element->index=0;
    sensor_element->flag=false;
    for(i=0;i<RUN_AVG_LENGTH;i++){sensor_element->last_sensor_data[i]=0;} // clear the sensor_element content   
    
    while (fscanf(fp_sensor_map,"%" SCNu16 " %" SCNu16 "\n", &sensor_element->room_id, &sensor_element->sensor_id) != EOF) { //room id and sensor id are all unsigned short int.
        dpl_insert_sorted(list_mgr, (void*)sensor_element, true);                                             // insert read sensor id and room id into List and make a deep copy
    }                                                                                             //SCNu16 make it restric 16 bits, not changing with different os
    
    
    while((fread(&data->id, sizeof(sensor_id_t), 1, fp_sensor_data)==1)
        &&(fread(&data->value, sizeof(sensor_value_t), 1, fp_sensor_data)==1)
        &&(fread(&data->ts, sizeof(sensor_ts_t), 1, fp_sensor_data)==1))
    {
        dummy_element = find_sensor_element_by_id(data->id);         //get the element with certain sensor_id
        if (dummy_element != NULL) {
            
            dummy_element->last_sensor_data[dummy_element->index] = data->value; // if the array is full then start overwrite the first one
            if(dummy_element->index<RUN_AVG_LENGTH-1) {dummy_element->index=(dummy_element->index)+1;}
            else {dummy_element->flag=true;dummy_element->index=0;}                 //if index = RUN_AVG_LENGTH it means the array is full
            dummy_element->last_modified = data->ts;
        
            if(dummy_element->flag==true)
            {
                
                dummy_element->running_avg = 0;          //reset the avg before calculate
                for(i=0;i<RUN_AVG_LENGTH;i++)
                {
                    dummy_element->running_avg+=dummy_element->last_sensor_data[i];
                }
                dummy_element->running_avg=(dummy_element->running_avg)/RUN_AVG_LENGTH;
                
                if ((dummy_element->running_avg) > SET_MAX_TEMP) 
                {
                    fprintf(stderr, "Warning: room %" PRIu16 " is too hot!!!!\n", dummy_element->room_id);    //print uint16_t use %" PRIu16 "
                    
                }
                else if ((dummy_element->running_avg) < SET_MIN_TEMP) 
                {
                    fprintf(stderr, "Warning: room %" PRIu16 " is too cold!!!!\n", dummy_element->room_id);
                    
                }
            }  //14.01 27.96
            else if(dummy_element->flag==false)
            {
                dummy_element->running_avg = 0;          //reset the avg before calculate
                for(i=0;i<dummy_element->index;i++)
                {
                    dummy_element->running_avg+=dummy_element->last_sensor_data[i];
                }
                dummy_element->running_avg =(dummy_element->running_avg)/(dummy_element->index);
                
                if ((dummy_element->running_avg) > SET_MAX_TEMP) 
                {
                    fprintf(stderr, "Warning: room %" PRIu16 " is too hot!!!!\n", dummy_element->room_id);    //print uint16_t use %" PRIu16 "
                    
                }
                else if ((dummy_element->running_avg) < SET_MIN_TEMP) 
                {
                    fprintf(stderr, "Warning: room %" PRIu16 " is too cold!!!!\n", dummy_element->room_id);
                    
                }
            }
                                
        }
        
        
    }
    
    free(sensor_element);
    free(data);
    
}
/*
 * This method should be called to clean up the datamgr, and to free all used memory. 
 * After this, any call to datamgr_get_room_id, datamgr_get_avg, datamgr_get_last_modified or datamgr_get_total_sensors will not return a valid result
 */
void datamgr_free()
{
    dpl_free(&list_mgr,true);
}

uint16_t datamgr_get_room_id(sensor_id_t sensor_id)
{
sensor_element_t *dummy_element;
dummy_element = find_sensor_element_by_id(sensor_id); 
if(dummy_element!=NULL) return dummy_element->room_id;
else return -1;
}

sensor_value_t datamgr_get_avg(sensor_id_t sensor_id)
{
sensor_element_t *dummy_element;
dummy_element = find_sensor_element_by_id(sensor_id);
if(dummy_element!=NULL)
{
  if(dummy_element->flag==0){return 0;}
  else {return dummy_element->running_avg;}
}

else return 0;    
}

time_t datamgr_get_last_modified(sensor_id_t sensor_id)
{
sensor_element_t *dummy_element;
dummy_element = find_sensor_element_by_id(sensor_id);
if(dummy_element!=NULL) return dummy_element->last_modified;
else return 0;
}

int datamgr_get_total_sensors()
{
return dpl_size(list_mgr);
}

void * element_copy_mgr(void * element) {
    sensor_element_t* copy = malloc(sizeof (sensor_element_t));
    copy->sensor_id = ((sensor_element_t*)element)->sensor_id;
    copy->room_id = ((sensor_element_t*)element)->room_id;
    return (void *) copy;
}

void element_free_mgr(void ** element) {
    free(*element);
    *element = NULL;
}

int element_compare_mgr(void * x, void * y) {
    return ((((sensor_element_t*)x)->sensor_id < ((sensor_element_t*)y)->sensor_id) ? -1 : (((sensor_element_t*)x)->sensor_id == ((sensor_element_t*)y)->sensor_id) ? 0 : 1);
}

sensor_element_t * find_sensor_element_by_id(sensor_id_t sensor_id)
{
//    dplist_node_t *node;
//    node=dpl_get_first_reference(list );
//    for (; node != NULL; node = dpl_get_next_reference(list, node)) {
//        if(((sensor_element_t *)(dpl_get_element_at_reference(list, node)))->sensor_id == sensor_id) return (sensor_element_t *)(dpl_get_element_at_reference(list, node));
//    }
    int size,i;
    size=dpl_size(list_mgr);
    for(i=0;i<size;i++)
    {
        if(((sensor_element_t *)(dpl_get_element_at_index(list_mgr,i)))->sensor_id==sensor_id) return (sensor_element_t *)(dpl_get_element_at_index(list_mgr,i));
        }
    return NULL;
    }

//gcc -Wall -std=c11 -Werror -D SET_MIN_TEMP=14.01 -D SET_MAX_TEMP=27.96  main.c datamgr.c -L. -ldplist
