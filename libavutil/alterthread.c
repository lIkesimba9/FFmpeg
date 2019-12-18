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
    int flag = 1;
    AVCodecContext *avctx = (AVCodecContext *)param;
    X264Context *x4 = (X264Context *)(avctx->priv_data);

    int sockfd, new_sockfd; // слушает на fd , новое соеденение на newfd
    struct sockaddr_in host_addr,client_addr;
    socklen_t sin_size;
    int recv_length = 1, yes = 1;
    unsigned char buffer[4];
    memset(buffer,NULL,4);
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
    unsigned int nul_vall = 0;
    //int flag = 0;
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
        recv_length = recv(new_sockfd,&buffer,4,0);
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
            memset(buffer,NULL,4);
            recv_length = recv(new_sockfd,buffer,4,0);
            //void *p = buffer;
            unsigned int transit = *(unsigned int *)buffer;
             av_log(NULL,AV_LOG_ERROR,"K= %u\n",transit);
             transit = transit / 100;
             av_log(NULL,AV_LOG_ERROR,"K= %u\n",transit);
           // av_log(NULL,AV_LOG_INFO,"transit= %u\n",(transit > nul_vall? transit - nul_vall:nul_vall -transit));
            if ( /*( transit < 400 ) &&*/ ( transit > 300) && flag  )
            {           //    3165288575

                //float m_crf = 40;
                float m_crf = 27;
                av_log(NULL,AV_LOG_INFO,"crf= %f\n",x4->crf);

                pthread_mutex_lock(&m);
                // x4->crf  = m_crf; //work it
                // avctx->bit_rate = 250000;
                //avctx->bit_rate = 250000;
                // avctx->rc_max_rate = 250000;
                //x4->cqp = 40; //don't work it
                // x4->params.rc.f_rf_constant = m_crf;
                // x264_encoder_reconfig(x4->enc, &x4->params);
                //x4->params.rc.i_vbv_max_bitrate = 250;
                // avctx->rc_max_rate = 250000;
                //x264_encoder_reconfig(x4->enc, &x4->params);
                //avctx->bit_rate = 1000000;
                x4->crf = m_crf;
                //avctx->rc_max_rate = 1000000;
                //avctx->bit_rate = 1000000;
                pthread_mutex_unlock(&m);

                 av_log(NULL,AV_LOG_INFO,"crf= %f\n",x4->crf);
                 flag = 0;
                //av_log(NULL,AV_LOG_ERROR,"change bit_rate\n",NULL);

                //x4->bit_rate = 250000;
                // x4->rc_max_rate
                // x4->rc_buffer_size
            }
            /* else
            {

                int m_crf = 2;
                pthread_mutex_lock(&m);
                x4->crf  = m_crf;
                //avctx->rc_max_rate = 500000;
                pthread_mutex_unlock(&m);
            }*/
        }
        close(new_sockfd);

    }


}
/* Supported reconfiguration options (1-pass only):
     * vbv-maxrate
     * vbv-bufsize
     * crf
     * bitrate (CBR only) */
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
