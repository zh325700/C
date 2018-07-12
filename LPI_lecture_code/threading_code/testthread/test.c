#include <pthread.h>  
#include <unistd.h>  
#include <stdio.h>  

//void *thread()  
//{  
//    int i;  
//    for (i = 0; i < 10; ++i)  
//    {  
//        sleep(2);  
//        printf( "This in the thread : %d\n" , i );  
//    }  
//    return NULL;  
//}  
//  
//int main()  
//{  
//    pthread_t pth;               //pthread_t：pthread_t用于声明线程ID！
//    int i;  
//    int ret = pthread_create(&pth, NULL, thread, NULL);  
//      
//    pthread_join(pth, NULL);              // 这一行的作用是等待thread执行完他的函数以后才开始主函数的函数 
//    for (i = 0; i < 10; ++i)  
//    {  
//        sleep(1);  
//        printf( "This in the main : %d\n" , i );  
//    }  
//      
//    return 0;  
//}
int sum;  
void *runner (void *param);  
  
int main(int argc, char *argv[])  
{  
    pthread_t tid;/*线程标示符*/  
    pthread_attr_t attr;  
  
    if (argc != 2)/*如果参数不为2个*/  
    {  
        fprintf (stderr, "usage:a.out<integer value>\n");/*报错*/  
        return -1;  
    }  
    if (atoi(argv[1] ) < 0)  
    {  
        fprintf (stderr, "%d must be <= 0\n", atoi(argv[1]));  
        return -1;  
    }  
    pthread_attr_init(&attr); /*初始化,得到默认的属性值*/  
    pthread_create(&tid, &attr, runner, argv[1]);/*创建一个线程*/  
    pthread_join(tid, NULL);/*等待子线程执行完毕*/  
  
    printf ("sum = %d\n", sum);  
    return 0;  
}  
  
void *runner(void *param)/*子线程将会执行这个函数*/  
{  
    int i, upper = atoi(param);  
    sum = 0;  
    for (i = 1; i <= upper; i++)  
    {  
        sum += i;  
    }  
    pthread_exit(0);  
} 