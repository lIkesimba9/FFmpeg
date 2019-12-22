#include "ffback.h"
#include "string.h"
#include "sys/socket.h"
#include "netinet/in.h"
#include "arpa/inet.h"
#include "circular_buffer.h"
#include "log.h"
#include <time.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <stdio.h>
//#include "Queue.h"
#define LEN_COMMAND 4

typedef struct thread_param
{

    cbuf_handle_t cbuf;
    int m_run;

} thread_param;
double sumQueueUnsigned(Queue* q)
{
    NodeQ *cur_node = q->front;
    double sum = 0;
    while (cur_node->prev != NULL)
    {
        //printf("%d\n",sum);
        sum +=((double)((unsigned)(cur_node->data)) / q->size);
        cur_node = cur_node->prev;

    }
    sum += ((double)((unsigned)(cur_node->data)) / q->size);
    //printf("%d\n",sum);
    return sum;
}
double sumQueue(Queue* q)
{
    NodeQ *cur_node = q->front;
    double sum = 0;
    while (cur_node->prev != NULL)
    {
        //printf("%d\n",sum);
        sum +=((double)((cur_node->data)) / q->size);
        cur_node = cur_node->prev;

    }
    sum += ((double)((cur_node->data)) / q->size);
    //printf("%d\n",sum);
    return sum;
}
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
                fscanf(fp,"%s",ip);
#ifdef debug
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

    Queue *q = initQueue();
    Queue *sg = initQueue();
    unsigned int mas[100];
    int c_i = 0;
    cbuf_handle_t cbuf = ((thread_param *)param)->cbuf;
    int term = ((thread_param *)param)->m_run;
    //FILE *fp;
    int sockfd;
    int commad = 0;
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
    unsigned char val[4];
    memset(val,NULL,4);
    unsigned int sum = 0;
    int count = 0;
    int c_fl = 0;
    bool first_flag = false;
    int next = 0;
    bool send_flag = false;
    unsigned int nul_val = 0;
    double avg = 0;
    unsigned int min_m = 0;
    unsigned int max = 0;
    int count_new = 0;
    int front_proiz = 0;
    int back_proiz = 0;
    double y_1 = 0;
    double y_2 = 0;
    double cur_av = 0;
    double time_1 = 0;
    double time_2 = 0;
    bool detect = true;
    bool send_detect = false;
    struct timespec mtime_last_send;
    clock_gettime (CLOCK_REALTIME, &mtime_last_send);
    long seconds_last_send = 0;
    bool flag_wait = true;
    long wait_second = 0;
    double sg_val;
    int count_miss = 0;
    while (term == 1)
    {

        unsigned char data;
        c_fl++;
        /*
                int flag = circular_buf_get(cbuf,&data);
                if (flag == -1)
                        continue;
                if (data = 0xff)
                        syn++;
                else
                        syn = 0;
                if (syn == 4){*/
        for (int j = 0; j < 4;)
        {
            int flag_m = circular_buf_get(cbuf,&data);
            if ( flag_m == -1)
                continue;
            val[j] = data;
            j++;

        }

        if (!first_flag)
        {
            // av_log(NULL,AV_LOG_ERROR,"DETECT FIRST FLAG\n",NULL);


            unsigned int c_nul_val = *((unsigned int *)(val));
            if (c_nul_val > 0){
                nul_val =c_nul_val;
                min_m = c_nul_val;
                max = c_nul_val;
                cur_av = c_nul_val;
                first_flag = true;


                //   av_log(NULL,AV_LOG_ERROR,"max=%d min_m=%d\n",max,min_m);
            }
            else
                continue;
            continue;
        }
        if (c_i < 100)

        {
            unsigned int c_val = *((unsigned int *)(val));
            //  av_log(NULL,AV_LOG_ERROR,"cur_val=%d avg=%.3f max=%d min_m=%d\n",c_val,avg,max,min_m);
            if (c_val != 0)
            {
                cur_av = (cur_av) * (double)(c_i + 1) / (c_i + 2) + c_val / (double)(c_i + 2) ;
                //av_log(NULL,AV_LOG_ERROR,"cur_val=%d avg=%.3f",c_val,cur_av);
                enqueue(q,c_val); //- cur_av);
                avg += (c_val / 100);
                if (c_val > max)
                    max = c_val;
                if (c_val < min_m)
                {
                    av_log(NULL,AV_LOG_ERROR,"c_val=%d min_m=%d\n",c_val,min_m);
                    min_m = c_val;
                }


            }
            else
                continue;
            // av_log(NULL,AV_LOG_INFO,"c_val=%d max=%d min_m=%d\n",c_val,max,min_m);
            c_i++;
            //  min_m = min_m;
            //av_log(NULL,AV_LOG_ERROR,"DETECT %d\n",c_i);

            if (c_i == 100)
            {
                av_log(NULL,AV_LOG_ERROR,"BEGIN SUM\n",NULL);

                sg_val = sumQueueUnsigned(q);

                av_log(NULL,AV_LOG_ERROR,"END SUM Q SIZE IS %d\n",q->size);
            }

        }

        else
        {

            //av_log(NULL,AV_LOG_ERROR,"avg=%.3f cur_avg=%.3f",avg,cur_av);
            unsigned int c_val = *((unsigned int *)(val));
            //av_log(NULL,AV_LOG_INFO,"cur_val=%d avg=%.3f max=%d min_m=%d\n",c_val,avg,max,min_m);
            NodeQ *last_val = dequeue(q);

            sg_val = sg_val - (  (double)((unsigned int)(last_val->data))  / 100);
            sg_val = sg_val + ((double)(c_val ) / 100);
            enqueue(q,c_val); // - avg);
            //   av_log(NULL,AV_LOG_ERROR,"cur_val=%u avg=%.3f max=%u min_m=%u\n",c_val,avg,max,min_m);
            //  int sent_bytes = send(sockfd,(char *)&K,4,0);

            //            av_log(NULL,AV_LOG_ERROR,"BEFORE DETECT\n",NULL);


            if ( (c_val - avg) > 3.5 * ( (max - min_m )) )
            {

                av_log(NULL,AV_LOG_ERROR,"DETECT CHANGE CHANEL\n",NULL);

                if (detect)
                {
                    commad = 29;
                    int sent_bytes = send(sockfd,(char *)&commad,4,0);
                    av_log(NULL,AV_LOG_ERROR,"SEND CRF=29\n",NULL);

                    count_miss = 0;
                    send_flag = true;
                    flag_wait = true;
                }
                /* if (send_detect)
                {
                    commad = -1;
                    int sent_bytes = send(sockfd,(char *)&commad,4,0);
                    struct timespec cur_time;
                    clock_gettime (CLOCK_REALTIME, &cur_time);
                    long seconds_curr = cur_time.tv_sec;
                    seconds_last_send = seconds_curr;



                }*/
                detect = false;
                continue;

            }

            else {
                /*   if (count_miss < 200){
                    count_miss++;
                    continue;
                }*/
                if ( (commad != 0) && send_flag){

                    struct timespec cur_time;
                    clock_gettime (CLOCK_REALTIME, &cur_time);
                    long cur_second = 0;
                    if (flag_wait)
                    {
                        wait_second = cur_time.tv_sec;
                        flag_wait = false;
                    }
                    else
                    {
                        cur_second = cur_time.tv_sec;
                    }
                    if ( cur_second - wait_second > 20) //{
                    {
                            if (  (c_val - avg) >  1.3 * ( (max - min_m )) )
                            {
                                if (send_flag){
                                    commad = -2;
                                    int sent_bytes = send(sockfd,(char *)&commad,4,0);
                                    clock_gettime (CLOCK_REALTIME, &mtime_last_send);
                                    long seconds_last_send = mtime_last_send.tv_sec;

                                    av_log(NULL,AV_LOG_ERROR,"SEND -2!!!!!!\n",NULL);
                                    send_flag = false;
                                    detect = true;

                                }
                            }



                        if (count_new == 0)
                        {
                            struct timespec mt1;
                            clock_gettime (CLOCK_REALTIME, &mt1);
                            long seconds = mt1.tv_sec;
                            long ns = mt1.tv_nsec;
                            time_1 = (double)seconds + (double)ns/(double)1000000000;
                            y_1 = sg_val;
                        }
                        if (count_new == 100)
                        {
                            struct timespec mt1;
                            clock_gettime (CLOCK_REALTIME, &mt1);
                            long seconds = mt1.tv_sec;
                            long ns = mt1.tv_nsec;
                            time_2 = (double)seconds + ns/1000000000.;
                            y_2 = sg_val;

                            av_log(NULL,AV_LOG_ERROR,"K=%.3f\n", ((y_2 - y_1) / ( (time_2 -time_1) ) ) / 100);
                            int K = ( (y_2 - y_1) /  (time_2 -time_1) ) / 100;



                            if(  (  (seconds -seconds_last_send) > 3) )
                            {
                                av_log(NULL,AV_LOG_ERROR,"DIFF TIME=%lld\n", ( seconds -seconds_last_send ));
                                seconds_last_send = seconds;





                                if (send_flag){
                                    commad = 1;
                                    int sent_bytes = send(sockfd,(char *)&commad,4,0);
                                    clock_gettime (CLOCK_REALTIME, &mtime_last_send);
                                    long seconds_last_send = mtime_last_send.tv_sec;

                                    av_log(NULL,AV_LOG_ERROR,"SEND 1!!!!!!\n",NULL);

                                }








                            }
                            count_new = 0;

                        }
                        count_new++;
                    }
                    else
                        continue;
                }




                /*   if (count_new == 60 )
                {
                    // sum (q[i]/100)  / sec[win_size] - sec[0]
                    //  av_log(NULL,AV_LOG_ERROR,"DETECT CHANGE CHANEL\n",NULL);

                    struct timespec mt1;
                    clock_gettime (CLOCK_REALTIME, &mt1);
                    long seconds = mt1.tv_sec;
                    long ns = mt1.tv_nsec;
                    time_1 = (double)seconds + (double)ns/(double)1000000000;
                    y_1 = sg_val;

                }
                if (count_new == 160)
                {

                    struct timespec mt1;
                    clock_gettime (CLOCK_REALTIME, &mt1);
                    long seconds = mt1.tv_sec;
                    long ns = mt1.tv_nsec;
                    time_2 = (double)seconds + ns/1000000000.;

                    y_2 = sg_val;
                    av_log(NULL,AV_LOG_ERROR,"cur_val=%u avg=%.3f max=%u min_m=%u sg_val=%.3f\n",c_val,avg,max,min_m,sg_val);
                    av_log(NULL,AV_LOG_ERROR,"time=%.3f\n",time_2 -time_1);
                    av_log(NULL,AV_LOG_ERROR,"K=%.3f\n",(y_2 - y_1) / ( (time_2 -time_1) ));
                    unsigned int K = (y_2 - y_1) /  (time_2 -time_1);

                }
                count_new++;
                    */
                /*  if (count_new > 200)
                {
                    av_log(NULL,AV_LOG_ERROR,"cur_val=%u avg=%.3f max=%u min_m=%u\n",c_val,avg,max,min_m);
                    av_log(NULL,AV_LOG_ERROR,"time=%.3f\n",time_2 -time_1);
                    av_log(NULL,AV_LOG_ERROR,"K=%.3f\n",(y_2 - y_1) / ( (time_2 -time_1) ));
                    av_log(NULL,AV_LOG_ERROR,"y_1=%.3f y_2=%.3f\n",y_1,y_2);
                }*/
            }

        }




    }

    return;
}
