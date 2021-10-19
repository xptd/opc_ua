/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-24 16:17:46
 * @LastEditors: xptd
 * @LastEditTime: 2021-10-19 14:40:50
 */
#include "simatic/simatic.h"



static void usage(void) {
    printf("Usage: \nconnect <server-url:opc.tcp:192.168.15:4840> \n"
           "start:start servo motor \n"
           "stop:stop servo motor \n");
}

int connect_simatic_server(const char* endpoint_url)
{    
    if(UA_STATUSCODE_GOOD ==client_connect(endpoint_url))
        return 0;
    else
        return -1;
}

int main(int argc ,char* argv[])
{
    if (argc < 2)
    {
        usage();
        return EXIT_FAILURE;
    }
    
    if (strcmp(argv[1], "connect") == 0)
    {

        if (strncmp(argv[2], SIMENS_OPC_SERVER, strlen(SIMENS_OPC_SERVER)))
        {
            fprintf(stdout,"param err\n");
            usage();
            return EXIT_FAILURE;
        }
        fprintf(stdout,"connect server:%s:",argv[2]);
        if(connect_simatic_server(argv[2]) == 0)
            fprintf(stdout,"successed!");
        else
            fprintf(stdout,"failed!");
    }

}