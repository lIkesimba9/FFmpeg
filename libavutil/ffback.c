#include "ffback.h"
#include "string.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "circular_buffer.h"
#include "log.h"
#include "time.h"
#include <stdio.h>
#define PORT 7890
typedef struct thread_param
	{

cbuf_handle_t cbuf; 
int m_run;

	} thread_param;
void* send_back(void* param)
{
	cbuf_handle_t cbuf = ((thread_param *)param)->cbuf;
	int term = ((thread_param *)param)->m_run;
	FILE *fp;
	fp = fopen("/home/user/data.txt","wt");
	int sockfd;
	struct sockaddr_in target_addr;
	char buffer[] = "1";
	if((sockfd = socket(PF_INET,SOCK_STREAM,0))== -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR OPEN SOCKET\n");
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(PORT);
	target_addr.sin_addr.s_addr = inet_addr("127.0.0.1");//"10.0.0.1");//INADDR_LOOPBACK);		
	memset(&(target_addr.sin_zero),'\0',8);
	if (connect(sockfd,(struct sockaddr *)&target_addr,sizeof(struct sockaddr)) == -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR CONNECT\n");
	int sent_bytes = send(sockfd,buffer,strlen(buffer),0);
	int syn = 0;
	unsigned char val[8];
	memset(val,NULL,8);
	double sum = 0;
	int count = 0;
	int c_fl = 0;
	while (term == 1)
	{

		unsigned char data;
		c_fl++;
		int j = 0;
		int flag = circular_buf_get(cbuf,&data);
		if (flag == -1)
			continue;
		if (data = 0xff)
			syn++;
		else 
			syn = 0;
		if (syn == 4){
			for (; j < 8;)
			{
				int flag_m = circular_buf_get(cbuf,&data);
				if ( flag_m == -1)
					continue;
				val[j] = data;			
				j++;
			}
			if (count < 10)
			{
				void *p = val;
				double av_diff = *((double *)(val));
				fprintf(fp,"%f\n",av_diff);
				sum += av_diff;
				count++;
			
			}
			if (count == 10)
			{
			//int sent_bytes = send(sockfd,(char *)&sum,8,0);
			//av_log(NULL,AV_LOG_INFO,"COUNT SEND %d\n",sent_bytes);	
			//fflush(fp);
			sum = 0;
			count = 0;
			}
			syn = 0;
		}
			
	}
fclose(fp);
return;
}
