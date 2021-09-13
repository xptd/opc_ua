/*================================================================
*   Copyright (C) 2021 PANDA Ltd. All rights reserved.
*   
*   文件名称：client.c
*   创 建 者：xptd
*   创建日期：2021年09月13日
*   描    述：
*
================================================================*/


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
