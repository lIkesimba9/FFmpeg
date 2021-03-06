#include "ffback.h"
#include "string.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "circular_buffer.h"
#include "log.h"
#include <stdio.h>
/*ffback::ffback()
{

}*/
//#define IP "127.0.0.1" 
#define PORT 7890
void* send_back(void* param)
{
	cbuf_handle_t cbuf = param;
	int sockfd;
	struct sockaddr_in target_addr;
	char buffer[] = "1";
	if((sockfd = socket(PF_INET,SOCK_STREAM,0))== -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR OPEN SOCKET\n");
	target_addr.sin_family = AF_INET;
	target_addr.sin_port = htons(PORT);
	target_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);		
	memset(&(target_addr.sin_zero),'\0',8);
	if (connect(sockfd,(struct sockaddr *)&target_addr,sizeof(struct sockaddr)) == -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR CONNECT\n");
	int sent_bytes = send(sockfd,buffer,strlen(buffer),0);
	while (1)
	{
		char data;
				
		int flag = circular_buf_get(cbuf,&data);
		if (flag != -1){
			av_log(NULL,AV_LOG_INFO,"SEND SYM %c\n",data);	
				
			int sent_bytes = send(sockfd,data,1,0);
			
		}
	}
	//close(sockfd);
	return;
}
