#include <string.h>
#include <stdio.h>
void pars_conn_param(int port,char *ip)
{
	FILE *fp;
	fp = fopen("/etc/ffback.cfg","rt");
	if (fp == NULL)
		printf("ERROR OPEN FILE");
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
                printf("%s",ip);
                #endif
			}
			if (memcmp(key,"port",strlen("port")) == 0)
			{
				int port_m;
				fscanf(fp,"%d",&port_m);
				printf("2\n");
                #ifndef debug
				printf("%d",port_m);
				#endif
			}
			i++;
		}
			
	}

}
int main()

{
int port;
char *ip = malloc(sizeof(char) * 15);
memset(ip,NULL,15);
pars_conn_param(&port,ip);


}
