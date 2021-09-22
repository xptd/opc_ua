/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 11:22:46
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-22 14:11:21
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"
#include "open62541.h"

static void sig_handler()
{
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "server terminated");
}

// int main(int argc ,char* argv[])
// {
//     signal(SIGINT, sig_handler);
//     signal(SIGTERM, sig_handler);
//     UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT ,"hello xptd for opc_ua_client test");
//     UA_Client *client = UA_Client_new();
//     UA_ClientConfig_setDefault(UA_Client_getConfig(client));
//     UA_Client_connect(client,"opc.tcp://localhost:4840");
//     UA_Client_disconnect(client);
//     UA_Client_delete(client);

//     return EXIT_SUCCESS;
// }

void clien_dump_endpoints(size_t endpoint_nums, UA_EndpointDescription *endpoint_des)
{
    if (endpoint_des == NULL || 0 == endpoint_nums)
    {
        return;
    }
    //dump endpoints info
    for (size_t i = 0; i < endpoint_nums; i++)
    {
        UA_EndpointDescription *endpoint = &endpoint_des[i];
        printf("\n\tEndpoint[%lu]:", (unsigned long)i);
        printf("\n\t\tEndpoint URL: %.*s", (int)endpoint->endpointUrl.length, endpoint->endpointUrl.data);
        printf("\n\t\tTransport profile URI: %.*s", (int)endpoint->transportProfileUri.length,
               endpoint->transportProfileUri.data);
        printf("\n\t\tSecurity Mode: ");
        switch (endpoint->securityMode)
        {
            case UA_MESSAGESECURITYMODE_INVALID:
                printf("Invalid");
                break;
            case UA_MESSAGESECURITYMODE_NONE:
                printf("None");
                break;
            case UA_MESSAGESECURITYMODE_SIGN:
                printf("Sign");
                break;
            case UA_MESSAGESECURITYMODE_SIGNANDENCRYPT:
                printf("Sign and Encrypt");
                break;
            default:
                printf("No valid security mode");
                break;
        }
        printf("\n\t\tSecurity profile URI: %.*s", (int)endpoint->securityPolicyUri.length,
               endpoint->securityPolicyUri.data);
        printf("\n\t\tSecurity Level: %d", endpoint->securityLevel);
    }
    fprintf(stdout,"\n");
}

UA_StatusCode client_get_endpoint(const char *server_url, size_t *endpoint_nums, UA_EndpointDescription **endpoint_des)
{

    if (server_url == NULL || endpoint_nums == NULL ||endpoint_des == NULL )
    {
        return UA_STATUSCODE_BADARGUMENTSMISSING;
    }
    UA_StatusCode retval;
    UA_Client *client = UA_Client_new();
    if(client == NULL)
    {
        return UA_STATUSCODE_BAD;
    }    
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client,server_url);
    if(retval != UA_STATUSCODE_GOOD)
    {
        UA_Client_delete(client);
        return retval;
    }
    return UA_Client_getEndpoints(client,server_url,endpoint_nums,endpoint_des);
}

UA_StatusCode client_connect(const char* endpoint_url)
{
    UA_StatusCode  retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, endpoint_url);
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    return retval;
}
