#include "alterthread.h"
#include "log.h"
/*
alerthread::alerthread()
{

}
*/

static pthread_mutex_t m;



void server_init(void *param)
{

	X264Context *x4 = (X264Context *)param;
	
	int sockfd, new_sockfd; // слушает на fd , новое соеденение на newfd
	struct sockaddr_in host_addr,client_addr;
	socklen_t sin_size;
	int recv_length = 1, yes = 1;
	unsigned char buffer[1024];
	memset(buffer,NULL,1024);	
	if ((sockfd = socket(PF_INET,SOCK_STREAM,0)) == -1 )
		av_log(NULL,AV_LOG_ERROR,"ERROR OPEN SERVER SOCKET\n");
	if (setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR SETSOCKOPT\n");
	host_addr.sin_family = AF_INET;
	host_addr.sin_port = htons(PORT);
	host_addr.sin_addr.s_addr = 0;
	memset(&(host_addr.sin_zero),'\0',8);
	
	if (bind(sockfd,(struct sockaddr *)&host_addr,sizeof(struct sockaddr)) == -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR BINDING SOCKET\n");
	else 
		av_log(NULL,AV_LOG_INFO,"SERVER BIND SOCKET\n");
	if (listen(sockfd,5) == -1)
		av_log(NULL,AV_LOG_ERROR,"ERROR LISTING\n");
	else
		av_log(NULL,AV_LOG_INFO," LISTIN\n");
	//sleep(30);
	while (1)
	{
		sin_size = sizeof(struct sockaddr_in);
		new_sockfd = accept(sockfd,(struct sockaddr *)&client_addr, &sin_size);
		if (new_sockfd == -1)
			av_log(NULL,AV_LOG_ERROR,"ERROR ACCEPT CONNECTION\n");
		else 
			av_log(NULL,AV_LOG_INFO,"CONNECT\n");
		//printf("server: got connection from %s port %d\n",inet_ntoa(client_addr.sin_addr),ntohs(client_addr.sin_port));
		//send(new_sockfd,"Hello\n",5,0);
		recv_length = recv(new_sockfd,&buffer,1024,0);
		av_log(NULL,AV_LOG_INFO,"RECV SYM %c\n",buffer[0]);	
		while (recv_length > 0)
		{
			//printf("RECV: %d bytes\n",recv_length);
			//printf("RECV: %s\n",buffer);
			/*int m_crf = 20;
			m_crf = buffer[0]-'0';
        		pthread_mutex_lock(&m);
			x4->crf  = m_crf;
   			pthread_mutex_unlock(&m);*/
			memset(buffer,NULL,1024);	
			recv_length = recv(new_sockfd,&buffer,1024,0);
			void *p = buffer;
			double av_diff = *(double *)p;
			av_log(NULL,AV_LOG_INFO,"av_diff = %f\n",av_diff);	
			if ( av_diff > 0 )
			{
				
				int m_crf = 2;
        			pthread_mutex_lock(&m);
				x4->crf  = m_crf;
   				pthread_mutex_unlock(&m);
			}
			else 
			{
			
				int m_crf = 20;
        			pthread_mutex_lock(&m);
				x4->crf  = m_crf;
   				pthread_mutex_unlock(&m);
			}
		}
		close(new_sockfd);
	
	}

	
}
void* change_param(void *param) {

	pthread_mutex_init(&m,NULL);
	server_init(param);
	/*sleep(10);
	FILE *fp;
	fp = fopen("/tmp/PARAM.txt","r");
	int crf_m = 20;
        //av_log(NULL, AV_LOG_INFO, " before cfr=%d\n",crf_m);
        fscanf(fp,"%d",&crf_m);
        //av_log(NULL, AV_LOG_INFO, " after cfr=%d\n",crf_m);
	fclose(fp);
        pthread_mutex_lock(&m);
	x4->crf  = crf_m;
   	pthread_mutex_unlock(&m);*/
	


	pthread_mutex_destroy(&m);
	pthread_exit(0);

}
