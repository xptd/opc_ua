/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 11:22:46
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-23 16:19:52
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "client.h"
#include "open62541.h"


// static void sig_handler()
// {
//     UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "server terminated");
// }

// // int main(int argc ,char* argv[])
// // {
// //     signal(SIGINT, sig_handler);
// //     signal(SIGTERM, sig_handler);
// //     UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT ,"hello xptd for opc_ua_client test");
// //     UA_Client *client = UA_Client_new();
// //     UA_ClientConfig_setDefault(UA_Client_getConfig(client));
// //     UA_Client_connect(client,"opc.tcp://localhost:4840");
// //     UA_Client_disconnect(client);
// //     UA_Client_delete(client);

// //     return EXIT_SUCCESS;
// // }

void print_nodeid(const UA_NodeId *nodeid)
{
    if (UA_NodeId_isNull(nodeid))
    {
        return;
    }
    UA_String id_str;
    UA_String_init(&id_str);
    if (UA_NodeId_print(nodeid, &id_str) != UA_STATUSCODE_GOOD)
    {
        return;
    }
    fprintf(stdout, "nodeid:%.*s\n", (int)id_str.length, id_str.data);
}

void client_dump_endpoints(size_t endpoint_nums, UA_EndpointDescription *endpoint_des)
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

UA_StatusCode client_browse_name2nodeid_single(
    UA_Client *client, 
    UA_NodeId start_nodeid, 
    const char* paths_browsename[], 
    UA_Int32 indexs[], 
    UA_UInt32 refer_ids[], 
    size_t path_nums, 
    UA_NodeId *target_nodeid)
{

    UA_StatusCode retval;
    if (UA_NodeId_isNull(&start_nodeid) || NULL == paths_browsename || NULL == indexs || NULL == refer_ids || path_nums <= 0)
    {
        return UA_STATUSCODE_BADPARENTNODEIDINVALID;
    }

    UA_BrowsePath browse_path;
    UA_BrowsePath_init(&browse_path);
    browse_path.startingNode = start_nodeid;
    browse_path.relativePath.elementsSize = path_nums;
    browse_path.relativePath.elements = (UA_RelativePathElement *)UA_Array_new(path_nums, &UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]);
    if (NULL == browse_path.relativePath.elements)
    {
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }
    for (size_t i = 0; i < path_nums; i++)
    {
        UA_RelativePathElement *tmp_element = &browse_path.relativePath.elements[i];
        UA_RelativePathElement_init(tmp_element);
        tmp_element->referenceTypeId = UA_NODEID_NUMERIC(0, refer_ids[i]);
        tmp_element->targetName = UA_QUALIFIEDNAME_ALLOC(indexs[i], paths_browsename[i]); //notice: must "alloc,otherwise UA_BrowsePath_clear() segment fault"
    }

    UA_TranslateBrowsePathsToNodeIdsRequest translate_req;
    UA_TranslateBrowsePathsToNodeIdsRequest_init(&translate_req);
    translate_req.browsePathsSize = 1;
    translate_req.browsePaths = &browse_path;

    UA_TranslateBrowsePathsToNodeIdsResponse translate_res;
    UA_TranslateBrowsePathsToNodeIdsResponse_init(&translate_res);

    translate_res = UA_Client_Service_translateBrowsePathsToNodeIds(client, translate_req);
    retval = translate_res.responseHeader.serviceResult;

    if (retval == UA_STATUSCODE_GOOD)
    {
        //translate_res.results:same browsename 2 nodeid
        if ((translate_res.resultsSize == 1) /*&& (translate_res.results[0].targetsSize == 1)*/)
        {
            UA_NodeId_copy(&translate_res.results[0].targets[1].targetId.nodeId, target_nodeid);
        }
    }

    // UA_Array_delete(browse_path.relativePath.elements,path_nums,&UA_TYPES[UA_TYPES_RELATIVEPATHELEMENT]);
    // //UA_TranslateBrowsePathsToNodeIdsRequest_clear(&translate_req);
    UA_BrowsePath_clear(&browse_path);
    UA_TranslateBrowsePathsToNodeIdsResponse_clear(&translate_res);
    return retval;
}
