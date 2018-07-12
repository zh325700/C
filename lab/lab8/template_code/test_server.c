#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include "config.h"
#include "tcpsock.h"
#include <poll.h>
#include <unistd.h>
#include "dplist.h"

#define PORT 5678
  // state the max. number of connections the server will handle before exiting
#ifndef TIMEOUT
  #define TIMEOUT 5
#endif
#ifndef MAX_CONN
  #define MAX_CONN 5
#endif
/* Implements a sequential test server (only one connection at the same time)
 */
 
dplist_t * list = NULL; 
void* element_copy(void * element);          // because you don't know the structure of the socket here, so you can no implement these method
void element_free(void ** element);
int element_compare(void * x, void * y);
 


int main( void )
{
  tcpsock_t * server, * client;
  sensor_data_t data;
  int bytes, result;
  int conn_counter = 0;              
  int  fd,i;
  bool no_new_con_flag=true;            // to indicate if there is new connection
  short revents;
  FILE *data_file;
  tcpsock_t * delet_client,* current_client;
  time_t current_ts,* socket_ts,*change_ts,* sever_ts;
  change_ts=(time_t *)malloc(sizeof(time_t));
  socket_ts=(time_t *)malloc(sizeof(time_t));
  list = dpl_create(element_copy, element_free, element_compare);         // create a list to store the connections (sockets)
  
  printf("Test server is started\n");
  if (tcp_passive_open(&server,PORT)!=TCP_NO_ERROR) exit(EXIT_FAILURE);
//    no_new_con_flag=true; // make the flag tre againpthread_barrier_wait
    
    tcp_get_tp(server,&sever_ts);
    time(sever_ts);
  
    while(1)
    {
        struct pollfd *pfds;
        pfds = (struct pollfd *)calloc(conn_counter+1,sizeof(struct pollfd));
        //struct pollfd pfds[conn_counter+1];                                        //the size is conn_counter+1 because I store the old socket inside to 老的文件描述符表示客户端的连接 listen to new socket
        
        for(int i=0;i<conn_counter+1;i++)      // add socket into the pollfd array 第一个是sever 其他的是 client
        {
                if(i==0){tcp_get_sd(server,&fd);pfds[0].fd = fd;pfds[0].events = POLLIN;}
                else{
                current_client=(tcpsock_t *)dpl_get_element_at_index(list, i-1 );
                tcp_get_sd(current_client,&fd);
                pfds[i].fd = fd;
                pfds[i].events = POLLIN;}
        }
        
        if(conn_counter==0)                   // to test if sever been not connected for timeout
        {
            time(&current_ts);
            if(current_ts-*sever_ts>=TIMEOUT)
                {
                    if (tcp_close( &server )!=TCP_NO_ERROR) exit(EXIT_FAILURE);
                    printf("server quit because of timeout\n");
                    return 0;
                }
        }
            
   
        i = poll(pfds, conn_counter+1, 0);
        
        if (i == -1) {
                perror("poll");
                exit(EXIT_FAILURE);
            }
        for (int i = 0; i <= conn_counter; i++)       // i 代表的是 list array 有多少个socket
         {
            if(i!=0)
            {
            revents=pfds[i].revents;
            time(&current_ts);        // get current time
            
            
            current_client = (tcpsock_t *)dpl_get_element_at_index(list, i-1 );
            tcp_get_tp(current_client,&socket_ts);                      // get the timestamp of the client 把现在client的timestamp拿出来放在socket_ts里面
            if((current_ts-*socket_ts)>=TIMEOUT)     // 这里改一下下
              {
                delet_client = dpl_get_element_at_index(list, i-1 );
                tcp_close(&delet_client);
                dpl_remove_at_index(list, i-1, false);
                conn_counter=dpl_size(list);
                if(conn_counter==0){time(sever_ts);}                   //refresh sever timestamp when one client close
                printf("one connection closed because of timeout\n");
                free(pfds);
                break;     
              }

            
            if ((revents & POLLIN) == POLLIN) {     
            
                      tcp_get_tp(current_client,&change_ts);//在这里给list 里面的client 的timestamp 刷新值
                      time(change_ts);
                            
                                  // read sensor ID
                      
                      bytes = sizeof(data.id);
                      result = tcp_receive(dpl_get_element_at_index(list, i-1),(void *)&data.id,&bytes);
                      // read temperature
                      bytes = sizeof(data.value);
                      result = tcp_receive(dpl_get_element_at_index(list, i-1),(void *)&data.value,&bytes);
                      // read timestamp
                      bytes = sizeof(data.ts);
                      result = tcp_receive( dpl_get_element_at_index(list, i-1), (void *)&data.ts,&bytes);
                      if ((result==TCP_NO_ERROR) && bytes) 
                      {
                        printf("sensor id = %" PRIu16 " - temperature = %g - timestamp = %ld\n", data.id, data.value, (long int)data.ts);
                        data_file=fopen("sensor_data_recv","wb");
                        if(!data_file){printf("file open failed");}
                        fwrite(&data.id,sizeof(uint16_t),1,data_file);
                        fwrite(&data.value,sizeof(double),1,data_file);
                        fwrite(&data.ts,sizeof(time_t),1,data_file);
                        fclose(data_file);
                      }
                      else if (result==TCP_CONNECTION_CLOSED) 
                        {delet_client = dpl_get_element_at_index(list, i-1 );
                        tcp_close(&delet_client);
                        dpl_remove_at_index( list, i-1, false);
                        conn_counter=dpl_size(list);
                        if(conn_counter==0){time(sever_ts);}
                        printf("one connection closed\n");
                        free(pfds);
                        break;                                    // if one connection is broken, need to poll again 
                        }    
                //      else
                //        {printf("good good study");}
                
                                         }
            
            }
           if(((pfds[0].revents) & POLLIN )== POLLIN)
               {printf("new connection is coming\n");
//               no_new_con_flag=false;
               free(pfds);
               if (tcp_wait_for_connection(server,&client)!=TCP_NO_ERROR) {exit(EXIT_FAILURE);}
                printf("Incoming client connection\n");
                tcp_get_tp(client,&change_ts);
                time(change_ts);//record the time connection was established
                list = dpl_insert_at_index(list, client, MAX_CONN, false);             // insert the socket into the list, so next time new socket connected, we will not lose the previous one
                conn_counter=dpl_size(list);                    // get the number of the connection
               break;}        // there is new connection then break out for loop
         } 
     
    } 
  

  if (tcp_close( &server )!=TCP_NO_ERROR) exit(EXIT_FAILURE);
  printf("Test server is shutting down\n");
  free(change_ts);
  free(socket_ts);
  dpl_free(&list,false);
  return 0;
}


void * element_copy(void * element) {
    return NULL;
}

void element_free(void ** element) {
    free(*element);
    *element = NULL;
}

int element_compare(void * x, void * y) {
    return 0;
}


