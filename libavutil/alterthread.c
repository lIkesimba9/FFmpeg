#include "alterthread.h"
#include "log.h"



static pthread_mutex_t m;
// this poly for range 20-30 crf ...
double crf_to_bitrate(float crf)

{
    //[-2.299, 10.886, -19.27, 33.15]
    double bitrate = 0;
    double poly[] = {-2.13, 178.72, -5056.6, 48613.58};
    int i = 3;
    int step = 0;
    for (i;i >= 0;i--)
    {
        bitrate +=  ( poly[i] * pow(crf,step) );
        step++;

    }
    return bitrate;
}
float bitrate_to_crf(double bitrate)
{

    float crf = 0;
    //[-2.299, 10.886, -19.27, 33.15]
    bitrate = bitrate/1000.;
    double poly[] = {-2.299, 10.886, -19.27, 33.15};
    int i = 3;
    int step = 0;
    for (i;i >= 0;i--)
    {
        crf +=  ( poly[i] * pow(bitrate,step) );
        step++;

    }
    return round(crf+0.5);
}


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
            //av_log(NULL,AV_LOG_INFO,"bitrate=%lld\n",avctx->bit_rate);
            //continue;
            memset(buffer,NULL,4);
            recv_length = recv(new_sockfd,buffer,4,0);
            //void *p = buffer;
            unsigned int transit = *(unsigned int *)buffer;
             av_log(NULL,AV_LOG_ERROR,"K= %u\n",transit);
             transit = transit / 100;
             av_log(NULL,AV_LOG_ERROR,"K= %u\n",transit);
            double diff = 0;
            if ( (transit > 0) && (transit < 200) )
                diff = 200;
            if ( (transit > 200) && (transit < 300) )
                diff = 300;
            if ( (transit > 300) && (transit < 400) )
                diff = 400;
            if ( (transit > 400) && (transit < 500) )
                diff = 500;
            if ( (transit > 500) && (transit < 600) )
                diff = 600;

            av_log(NULL,AV_LOG_INFO,"diff_bit= %f\n",diff);

            double cur_bit = crf_to_bitrate(x4->crf);
            av_log(NULL,AV_LOG_INFO,"cur_bit= %f\n",cur_bit);
            float new_crf = bitrate_to_crf(cur_bit - diff);
            av_log(NULL,AV_LOG_INFO,"new_ctf= %f\n",new_crf);


                pthread_mutex_lock(&m);

                x4->crf = new_crf;
                pthread_mutex_unlock(&m);

                 av_log(NULL,AV_LOG_INFO,"crf= %f\n",x4->crf);

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
