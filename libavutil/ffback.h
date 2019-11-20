#ifndef FFBACK_H
#define FFBACK_H
#include "queue.h"
/*class ffback
{
public:
    ffback();
};*/

void pars_conn_param(int *port,char *ip);
void* send_back(void *param);
double sumQueueUnsigned(Queue* q);
double sumQueue(Queue* q);
#endif // FFBACK_H
