#include<stdio.h>   
#include<stdlib.h>
#include<sys/types.h> 
#include<sys/stat.h>
#include<netinet/in.h>  
#include<sys/socket.h> 
#include<string.h>
#include<unistd.h>
#include<signal.h>
#include<sys/ipc.h>
#include<errno.h>
#include<sys/shm.h>
#include<time.h>
#include<pthread.h>

#define PORT 5678                   //端口号
#define SIZE 1024					//缓冲区大小
#define SIZE_SHMADD 2048			//内存共享区大小
#define BACKLOG 10					//客户端最大连接数量
int sockfd;                         //套接字描述符
int fd[BACKLOG];					//客户端套接字描述符
int i=0;							//计数变量
int j=0;							//计数变量
int k=0;							//计数变量
int strnum;							//计数变量
int recvnum;						//接收客户端变量
int sendnum;						//发送测试变量
//char *logstr;
//char name[6];
char code[5]="haha";
char xinhao[15]="***************";
/***************************/

//char test_buffer[SIZE],test_buf[SIZE]; //ser to cli;
char buffer_reg[SIZE];  //命令比较用的数组
/********************************/

typedef struct {
    char username[6];//用户名
    char password[6];//用户密码
}User;
int ifpass[100];//password right? right =1;密码是否正确变量
User users[100];//客户端用户信息
User usersin;//login
int userCount=0;//用户的数量
char username[30];//登陆人的名字
char password[50];//登陆人的密码

//***************************//


/*********套接字描述符*******/
int get_sockfd()
{    
	struct sockaddr_in server_addr;
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) //建立套接字 
	{    
		fprintf(stderr,"Socket error:%s\n\a",strerror(errno));          
		exit(1); 
	}
	else
	{
        printf("Socket successful!\n"); 
    }     /*sockaddr结构 */ 
    
    bzero(&server_addr,sizeof(struct sockaddr_in)); 
    server_addr.sin_family=AF_INET;                
    server_addr.sin_addr.s_addr=htonl(INADDR_ANY); 
    server_addr.sin_port=htons(PORT);  
    /*绑定服务器的ip和服务器端口号*/
    if(bind(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) //绑定地址 
    {      
    	fprintf(stderr,"Bind error:%s\n\a",strerror(errno));       
        exit(1);     
    } 
    else
    {
    printf("Bind successful!\n");    
    }  
     /* 设置允许连接的最大客户端数 */     
    if(listen(sockfd,BACKLOG)==-1)     //监听
    {    
     	  fprintf(stderr,"Listen error:%s\n\a",strerror(errno)); exit(1);  } else{
          printf("Listening.....\n"); 
    } 
    return sockfd;//返回套接字描述符
}

/*创建共享存储区*/
int shmid_create()
{    
	int shmid;     
	if((shmid = shmget(IPC_PRIVATE,SIZE_SHMADD,0777)) < 0)       
	{ 
	perror("shmid error!"); exit(1); 
	}
    else  
    printf("shmid success!\n");
    return shmid;
}//********************************log***********************************************/

/************************************************************************************/
int main(int argc, char *argv[]) 
{   
	char shmadd_buffer[SIZE_SHMADD],buffer[SIZE];     //内存共享区 
	struct sockaddr_in client_addr;  //客户端信息
    int sin_size;   
    pid_t ppid,pid;     //定义进程
    int new_fd; 
    int shmid;          //内存共享变量
    char *shmadd;		//内存数据缓冲区
    /***********共享内存**************/
    shmid = shmid_create();
    //映射共享内存
    shmadd = shmat(shmid, 0, 0);
    /*****创建套接字描述符***********/ 
    int sockfd = get_sockfd();
    /*循环接收客户端*/
    while(1)
    {   /* 服务器阻塞,直到客户程序建立连接 */      
      sin_size=sizeof(struct sockaddr_in);        
      if((new_fd=accept(sockfd,(struct sockaddr *)(&client_addr),&sin_size))==-1)  //建立连接       
      { 
      fprintf(stderr,"Accept error:%s\n\a",strerror(errno)); 
       exit(1);   
       }
      else{printf("Accept successful!\n"); } 
      
     fd[i++] = new_fd; //将客户端套接字描述符存入数组中
    // printf("%d\n",i-1);   
     printf("\n已连接了客户端%d : %s:%d \n",i , inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
       /*把界面发送给客户端*/
     memset(buffer,0,SIZE);
     strcpy(buffer,"\n——————————————————Welecom come char ———————————————————————\n");
     send(new_fd,buffer,SIZE,0);//向客户端发送信息
    //创建子进程客户端

    ppid = fork(); 
  //  while(1){
     if(ppid > 0)
    {
        //将加入的新客户发送给所有在线的客户端/
       recv(new_fd,buffer,SIZE,0);
       strcat( buffer," 进入了聊天室....");  
       for(i=0;i<BACKLOG;i++)//向所有客户端发送信息
       {  
         if(fd[i]!=-1)
         {  
              send(fd[i],buffer,strlen(buffer),0);  
         }  
       }   
        //创建子进程进行读写操作/
      pid = fork();    
       while(1)
       {
         if(pid >0)
         {
           //父进程用于接收信息/
           memset(buffer,0,SIZE);
           if((recv(new_fd,buffer,SIZE,0)) <= 0)//接收信息
           {
              close(new_fd);
              exit(1); 
            }	
            	recvnum=new_fd;//确定是哪个客户端
           		//printf("recvnum=%d\n",recvnum);
           			printf("客户端消息\n");
                   printf(" %s\n",buffer);
                   
                   /************************log********************/
                   if(strncmp("logout",buffer,6)==0)//检测是否是注册命令
                 // strnum=StringFind(buffer,"login");
                 // if(strnum!=-1)
                  {printf("注册中\n");
                  strnum=6;
                  //将用户信息存入结构体类型数组
                  	for(j=0;j<5;j++)
                  	{
                  	users[userCount].username[j]=buffer[strnum];
                  	strnum++;                 	
                  	}                	
                  	 printf("\nusername is %s\n",users[userCount].username);
                    for(j=0;j<5;j++)
                  	{
                  	users[userCount].password[j]=buffer[strnum];
                  	strnum++;                 	
                  	}                	
                  	 printf("\nuserpassword is %s\n",users[userCount].password);                    	                
                  	 userCount++;	 
                  }else if(strncmp("login",buffer,5)==0)//检测是否是登录命令
                    {
                  strnum=5;
                  printf("登录中\n");
                  //将登录信息存入待比较的结构体变量
                    for(j=0;j<5;j++)
                  	{
                  	usersin.username[j]=buffer[strnum];
                  	strnum++;                 	
                  	}
                  	for(j=0;j<5;j++)
                  	{
                  	usersin.password[j]=buffer[strnum];
                  	strnum++;                 	
                  	}    
                  	printf("\nusername=%s\n",usersin.username);
                  	printf("\npassword=%s\n",usersin.password);                 
                  	for(j=0;j<10;j++)
                  	{
                  	if(!strncmp(usersin.username, users[j].username,5))//判断用户名是否相同
                  		if(!strncmp(usersin.password, users[j].password,5))//检查密码是否相同
                  		{printf("password is right\n");ifpass[recvnum]=1;
                  		//printf("ifpass=%d,recvnum=%d",ifpass[recvnum],recvnum);
                  		}
                  		else
                  		{ printf("password wrong\n");ifpass[recvnum]=0;
                  		//printf("ifpass=%d,recvnum=%d",ifpass[recvnum],recvnum);
                  		}
                  	else
                  	//printf("username is wrong\n");
                      continue;            	
                  	}
                  		 
                  }else if((strncmp("p",buffer,1)==0)&&(ifpass[recvnum]))//检测是否是私聊命令
                  	{
                  		j=buffer[1]-0x30;//将ASCII码转换为整形数字
                  		printf("%d\n",j);
                  		/*****************fasongwenjian************************/
                  		for(k=0;k<4;k++) buffer_reg[k]=buffer[k+3];
                  		//printf("reg=%s\n",buffer_reg);
             			if((strncmp("file",buffer_reg,4)==0))////检测是否是文件传送命令
             			{memset(buffer_reg,0,SIZE);
             			//printf("file\n");
             			printf("文件操作中\n");
             			recv(new_fd,buffer_reg,SIZE,0);//接收信息
             			//printf("%s\n",buffer_reg);
             			send(fd[j],"file",SIZE,0);//向指定客户端发送文件命令
             			send(fd[j],buffer_reg,SIZE,0);//向指定客户端传送信息
             			} else
             			
             			/****************************************/
                  		{
                  		printf("私聊操作中\n");
                  		send(fd[j],xinhao,SIZE,0);
                  		//sendnum=send(fd[j],code,SIZE,0); 
                  		//printf("sendnum=%d\n",sendnum);
                  		//printf("fd=%d\n",fd[j]);
                  		send(fd[j],buffer,SIZE,0);//向指定客户端传送信息
                  		//send(j+4,buffer,SIZE,0);
                  		//send(j+4,xinhao,SIZE,0);
                  		send(fd[j],xinhao,SIZE,0);
                  		//printf("buffer=%s\n",buffer);
                  		}
                  	
                  	}else if((strncmp("qun",buffer,3)==0)&&(ifpass[recvnum]))//检测是否是群聊命令
                  	{
			printf("群聊操作中\n");
            memset(shmadd, 0, SIZE_SHMADD);
            strncpy(shmadd, buffer, SIZE_SHMADD);//将缓存区的客户端信息放入共享内存里
     				}
         }
         if(pid == 0)
         {
           //子进程用于发送信息/
           sleep(1);//先执 行父进程
           if(strcmp(shmadd_buffer,shmadd) != 0)
           {
              strcpy(shmadd_buffer,shmadd);
              if(new_fd  > 0)
              {
                 if(send(new_fd,shmadd,strlen(shmadd),0) == -1)//将共享区内容发送给客户端
                 {
                   perror("send");
                 }                
                 memset(shmadd, 0, SIZE_SHMADD);
                 strcpy(shmadd,shmadd_buffer); 
                // printf("\n666\n");               
              }
           }
         }
      }
     } 
   /*  if(ppid==0)
     {
             
 
        memset(test_buf,0,SIZE);
        fgets(test_buf,SIZE,stdin);
        strncat(test_buffer,test_buf,strlen(test_buf)-1);
        strcat(test_buffer,"\n"); 
        send(new_fd,test_buffer,SIZE,0);

     }*/
   // }//while(1)的外括号
   }   
 free(buffer);
   close(new_fd);//关闭套接字
   close(sockfd);
   return 0;
}
