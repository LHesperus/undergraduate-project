 #include<stdio.h>
#include<netinet/in.h>  
#include<sys/socket.h> 
#include<sys/types.h>
 #include<string.h>
#include<stdlib.h>
#include<netdb.h>
#include<unistd.h>
#include<signal.h>
#include<errno.h>
#include<time.h>
#include<string.h>  
#include<stdio.h> //C语言的I/O    
  
#define SIZE 1024
//void sendFile(void);

int main(int argc, char *argv[])
{

	time_t timer;   
	struct tm* t_tm;   
	time(&timer);   
	t_tm = localtime(&timer);   
	printf("today is %4d-%02d-%02d %02d:%02d:%02d\n", t_tm->tm_year+1900,   
	t_tm->tm_mon+1, t_tm->tm_mday, t_tm->tm_hour, t_tm->tm_min, t_tm->tm_sec);   


    pid_t pid;
    int sockfd,confd;
    char buffer[SIZE],buf[SIZE]; 
    char tempbuf[1024];
    struct sockaddr_in server_addr;
    struct sockaddr_in client_addr;
    struct hostent *host;
    short port;
    char *name;  
    //四个参数
    if(argc!=3) 
    { 
        fprintf(stderr,"Usage:%s hostname \a\n",argv[0]); 
        exit(1); 
    } 
       //使用hostname查询host 名字 
    if((host=gethostbyname(argv[1]))==NULL) 
    { 
        fprintf(stderr,"Gethostname error\n"); 
        exit(1); 
    } 
        port=atoi(argv[2]);
      
       /*客户程序开始建立 sockfd描述符 */ 
    if((sockfd=socket(AF_INET,SOCK_STREAM,0))==-1) 
    { 
        fprintf(stderr,"Socket Error:%s\a\n",strerror(errno)); 
        exit(1); 
    } else{
        printf("Socket successful!\n");
    }
        /*客户程序填充服务端的资料 */ 
    bzero(&server_addr,sizeof(server_addr)); // 初始化,置0
    server_addr.sin_family=AF_INET;          // IPV4
    server_addr.sin_port=htons(port);  // (将本机器上的short数据转化为网络上的short数据)端口号
    server_addr.sin_addr=*((struct in_addr *)host->h_addr); // IP地址
    /* 客户程序发起连接请求 */ 
    if(confd=connect(sockfd,(struct sockaddr *)(&server_addr),sizeof(struct sockaddr))==-1) 
    { 
        fprintf(stderr,"Connect Error:%s\a\n",strerror(errno)); 
        exit(1); 
    }else{
        printf("Connect successful!\n");
    }
    /*将客户端的名字发送到服务器端*/
 
     /*创建子进程，进行读写操作*/
     
    printf("input your name:");
    gets(name);
    printf("\nyour name is:%s\n",name);
    //send(sockfd,name,20,0);
   
    pid = fork();//创建子进程
     while(1)
    {
       
           if(pid > 0)/*父进程用于发送信息*/
           {       
          
         
         

           memset(buf,0,SIZE);
           
           fgets(buf,SIZE,stdin);
           
          
           
            strcat(buf,"<--");
            strcat(buf,name);
            strcat(buf,"\t");
           
           /*时间函数*/
            time_t timer;   
			struct tm* t_tm;   
			time(&timer);   
			t_tm = localtime(&timer);   

           printf(" ");
            strcat(buf, asctime(t_tm));
           
            send(sockfd,buf,SIZE,0);
 
             tempbuf[0]=buf[3];
              tempbuf[1]=buf[4];
               tempbuf[2]=buf[5];
                tempbuf[3]=buf[6];
                
           
        //   printf("\ntmpbuf is:%s",tempbuf);
            
           if(strncmp("file",tempbuf,4)==0)
           {

                bzero(buf, sizeof(buf));
               
                printf("你已经进入发送文件模式!\n");
		        #define FILE_NAME_MAX_SIZE  1024
		        #define BUFFER_SIZE  1024
		        
		        char file_name[FILE_NAME_MAX_SIZE + 1];  
				bzero(file_name, sizeof(file_name));  
				printf("请输入要传的文件名:\t");  
				scanf("%s", file_name); 
			  
				char bufferfile[BUFFER_SIZE];  
				bzero(bufferfile, sizeof(bufferfile)); 
			   
				FILE *fp = fopen(file_name, "r");  
				if (fp == NULL)  
				{  
				    printf("File:\t%s Not Found!\n", file_name);  
				}  
				else  
				{  
				    bzero(bufferfile, BUFFER_SIZE);  
				    int file_block_length = 0;  
		    while( (file_block_length = fread(bufferfile, sizeof(char), BUFFER_SIZE, fp))> 0)  
				    {  
				        
				        printf("length = %d\n", file_block_length);  
				      
				        send(sockfd,bufferfile,SIZE,0);
				       
				        
				        printf("发送文件内容为:\n%s",bufferfile);   
		
			   
				        bzero(bufferfile, sizeof(bufferfile));  
				    }  
				    fclose(fp);  
				    printf("File:\t%s发送完成！\n", file_name);  
				    
			//	    bzero(buffer, sizeof(buffer));
				    bzero(buf, sizeof(buf));
				} 
             
           }
           
     
        }
             
        else if(pid == 0)/*子进程用于接收信息*/
        {
             
               memset(buffer,0,SIZE);
               if(sockfd > 0)
               {
                 if((recv(sockfd,buffer,SIZE,0)) <= 0)
                 {
                  close(sockfd);
                  exit(1);
                 }
////////////////////////////////////////////////////////////////////////                 
                if(strncmp("file",buffer,4)==0)
             
            {
     
          FILE *fp = fopen("recfile.txt","w");  
        
             if(fp == NULL)  
               {  
                printf("File:zmp.txt Can Not Open To Write!\n");  
                exit(1);  
               } 
               
                bzero(buffer, sizeof(buffer));  
				int length = 0;  
				while(length = recv(sockfd, buffer, 1024, 0))  
				  {  
					if (length < 0)  
					{  
						printf("Recieve Data From Server %s Failed!\n", argv[1]);  
						break;  
					}  

				//int write_length = fwrite(buffer, sizeof(char), length, fp);  
				int write_length = fprintf(fp,"%s",buffer);
				
		//			if (write_length < length)  
		//			{  
		//		      printf("File Can Not Open To Write!\n"); 
		//				break;  
		//			}  
			  
   			     
   			       printf("接收文件内容为:%s\n",buffer);
   			       bzero(buffer, 1024);
   			       break;
   			      }  
  
				printf("接收保存文件成功!\n");  
			  
				// 传输完毕，关闭socket   
				fclose(fp);  
                 
             }
                 
////////////////////////////////////////////////////////////////////////                
               printf("%s\n",buffer);
            }
        }
    }   close(sockfd);
    return 0;    
}


