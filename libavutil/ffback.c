#include "ffback.h"
#include "string.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "circular_buffer.h"
#include "log.h"
#include "time.h"
#include <stdio.h>
/*ffback::ffback()
{

}*/
//#define IP "127.0.0.1" 
#define PORT 7890
void* send_back(void* param)
{
	cbuf_handle_t cbuf = param;
	//time_t t;
	//t = time(&t);
	int sockfd;
	struct sockaddr_in target_addr;
	char buffer[] = "1";
	if((sockfd = socket(PF_INET,SOCK_STREAM,0))== -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR OPEN SOCKET\n");
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(PORT);
	//inet_aton("10.0.0.2",&target_addr.sin_addr);
	target_addr.sin_addr.s_addr = inet_addr("10.0.0.2");//INADDR_LOOPBACK);		
	memset(&(target_addr.sin_zero),'\0',8);
	if (connect(sockfd,(struct sockaddr *)&target_addr,sizeof(struct sockaddr)) == -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR CONNECT\n");
	int sent_bytes = send(sockfd,buffer,strlen(buffer),0);
	int syn = 0;
	unsigned char val[8];
	memset(val,NULL,8);
	double sum = 0;
	int count = 0;
	while (1)
	{

		unsigned char data;
		int j = 0;
		int flag = circular_buf_get(cbuf,&data);
		if (flag == -1)
			continue;
		if (data = 0xff)
			syn++;
		else 
			syn = 0;
		if (syn == 4){
			//av_log(NULL,AV_LOG_INFO,"SUCCSES GET 4 0xff\n");	

			for (; j < 8;)
			{
				int flag = circular_buf_get(cbuf,&data);
				if ( flag == -1)
					continue;
				val[j] = data;			
				j++;
			}
			if (count < 100)
			{
				void *p = val;
				double av_diff = *((double *)(val));
				sum += av_diff;
				count++;
			
			}
			if (count == 100)
			{
			//if (time(NULL) > t + 5)
			//	{
			int sent_bytes = send(sockfd,(char *)&sum,8,0);
			av_log(NULL,AV_LOG_INFO,"COUNT SEND %d\n",sent_bytes);	
			//	t_time = time(NULL);
			//	}
			sum = 0;
			count = 0;
			}
		syn = 0;
		}
	/*	char data;
				
		int flag = circular_buf_get(cbuf,&data);
		if (flag != -1){
			//av_log(NULL,AV_LOG_INFO,"SEND SYM %c\n",data);	
				
			int sent_bytes = send(sockfd,data,1,0);
			av_log(NULL,AV_LOG_INFO,"COUNT SEND %d\n",sent_bytes);	
			
		}
	*/
	//int sent_bytes = send(sockfd,buffer,strlen(buffer),0);
	}
	//close(sockfd);
	return;
}
