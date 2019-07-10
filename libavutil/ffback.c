#include "ffback.h"
#include "string.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "circular_buffer.h"
#include "log.h"
#include "time.h"
#include <stdio.h>
typedef struct thread_param
	{

cbuf_handle_t cbuf; 
int m_run;

	} thread_param;
void pars_conn_param(int *port,char *ip)
{
	FILE *fp;
	fp = fopen("/etc/ffback.cfg","rt");
	if (fp == NULL)
		av_log(NULL,AV_LOG_ERROR,"ERROR LOAD IP CONFIG",NULL);
        char *data =(char *)malloc(sizeof(char) * 16);
	memset(data,NULL,16);
	fscanf(fp,"%s",data);
        #ifdef debug
	printf("%s\n",data);
	#endif
	if (memcmp(data,"[network]",strlen("[network]")) == 0)
	{
		int i = 0;
		while (i < 2)
		{
			char *key = malloc(sizeof(char) * 5 );
			memset(key,NULL,5);
			fscanf(fp,"%s",key);
                        #ifdef debug
			printf("%s\n",key);
			#endif
			if (memcmp(key,"ip",strlen("ip")) == 0)
			{
//				char *ip_m = malloc(sizeof(char) * 15);
//				memset(ip_m,NULL,15);
                                fscanf(fp,"%s",ip);
                                #ifdef debug
                //				printf("%s",ip_m);
                                printf("%s\n",ip);
                                #endif
			}
			if (memcmp(key,"port",strlen("port")) == 0)
			{
				fscanf(fp,"%d",port);
                                #ifndef debug
				printf("%d\n",*port);
				#endif
			}
			i++;
		}
			
	}
fclose(fp);

}

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
	char *IP_ADDR = malloc(sizeof(char) * 16);
	memset(IP_ADDR,NULL,16);
	int PORT;
	pars_conn_param(&PORT,IP_ADDR);
	target_addr.sin_port = htons(PORT);
	target_addr.sin_addr.s_addr = inet_addr(IP_ADDR);//"10.0.0.1");//INADDR_LOOPBACK);		
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
