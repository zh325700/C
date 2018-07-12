#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#define BUFSIZE 20
int main()
{
char name[20]="0";
char firstname[10]="0";
char secondname[10]="0";
char  str[10]="0";
int *year;
int old;
char fn[10]="0";
char sn[10]="0";
int age=0;


year=&old;
printf("please insert your firstname:\t");
scanf("%s",firstname);
printf("please insert your secondname:\t");
scanf("%s",secondname);
printf("your firstname and second name are respectivly:%s and %s\n",firstname,secondname);
printf("please insert your age\n");
scanf("%d",year);
printf("your age is %d\n",*year);

int i=0;
while(secondname[i])
   {
      str[i]=toupper(secondname[i]);
      i++;
   }

printf("after capitalize the secondname is %s and the str is %s\n",secondname,str);

if(strcasecmp(str,secondname)<0)//use strcmp if you care about the case of caractor
{
printf("so the str is less than seconname\n");
}
else if(strcasecmp(str,secondname)>0)
{
printf("so the str is larger than seconname\n");
}
else if(strcasecmp(str,secondname)==0)
{
printf("so the str is the same as seconname\n");
}
strcpy(name,secondname);
strncat(name,firstname,10);//strncat(char *a,char *b ,int n) 是把串b前边的n个字符直接连接到a后边
printf("the name is : %s\n",name);

//snprintf(name,BUFSIZE,"%s",firstname);
printf("the size of firstname is: %d\n",snprintf(name,BUFSIZE,"%s ",firstname));
printf("the size of secondname is: %d\n",snprintf(name+strlen(name),BUFSIZE,"%s ",secondname));
printf("the size of the age is: %d\n",snprintf(name+strlen(name),BUFSIZE,"%d ",*year));
printf("the information of user is : %s\n",name);

sscanf(name,"%s %s %d",fn,sn,&age);
printf("%s %s %d\n",fn,sn,age);
return 0;
}
