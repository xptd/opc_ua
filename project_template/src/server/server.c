/*================================================================
*   Copyright (C) 2021 PANDA Ltd. All rights reserved.
*   
*   文件名称：server.c
*   创 建 者：xptd
*   创建日期：2021年09月13日
*   描    述：
*
================================================================*/


#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "open62541.h"


static UA_Boolean server_running = UA_TRUE;

static void sig_handler()
{
    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_SERVER,"server terminated");
    server_running = UA_FALSE;
}


int main(int argc, char *argv)
{

    signal(SIGINT,sig_handler);
    signal(SIGTERM,sig_handler);
    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_SERVER,"hello xptd for opc_ua_server test");
    UA_StatusCode retval;
    UA_Server *server= UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));
    retval = UA_Server_run(server,&server_running);
    UA_Server_delete(server);

    return retval == UA_STATUSCODE_GOOD ? EXIT_FAILURE : EXIT_SUCCESS;
}
