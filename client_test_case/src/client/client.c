/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 11:22:46
 * @LastEditors: xptd
 * @LastEditTime: 2021-10-20 13:34:35
 */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "client/client.h"
//define my logger
const UA_Logger XPTD_LOGGER = {log_out, NULL, log_clear};
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

UA_StatusCode client_config_logger(const char* endpoint_url)
{

    if (log_init(LOG_CONFIG_FILE))
        return UA_STATUSCODE_BAD;   
    UA_StatusCode  retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig *config = UA_Client_getConfig(client);
    config->logger = XPTD_LOGGER;
    UA_ClientConfig_setDefault(config);
    UA_LOG_INFO(UA_Log_Stdout, UA_LOGCATEGORY_CLIENT, "client config log\n");
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

UA_StatusCode client_simplified_browse_node(UA_Client *client, UA_NodeId start_id)
{

    UA_StatusCode retval;
    UA_BrowseResponse browse_res;
    UA_BrowseResponse_init(&browse_res);
    UA_BrowseRequest browse_req;
    UA_BrowseRequest_init(&browse_req);
   
    // only one target --- browse_res.resultsSize should be 1
    browse_req.nodesToBrowseSize = 1;
    browse_req.nodesToBrowse = UA_BrowseDescription_new();
    if(browse_req.nodesToBrowse == NULL)
    {
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }
     //get all:refer part4
    browse_req.requestedMaxReferencesPerNode =0;
    /* 'root' node */
    browse_req.nodesToBrowse[0].nodeId = start_id;
    /* return everything */ 
    browse_req.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL;                  
    browse_res =  UA_Client_Service_browse(client, browse_req);
    retval = browse_res.responseHeader.serviceResult;
    //ASSERT_GOOD(retval);
    if(UA_STATUSCODE_GOOD != retval)
    {
        goto __done;
    }
   
    printf("%-9s %-16s %-16s %-16s\n", "NAMESPACE", "NODEID", "BROWSE NAME", "DISPLAY NAME");
    for (size_t i = 0; i < browse_res.resultsSize; ++i)
    {
        //fprintf(stdout,"browse_res.results[i].referencesSize:%ld\n",browse_res.results[i].referencesSize);
        // referencesSize:'object nums frist tiers' 
        for (size_t j = 0; j < browse_res.results[i].referencesSize; ++j)
        {
            UA_ReferenceDescription *ref = &(browse_res.results[i].references[j]);
            if ((ref->nodeClass == UA_NODECLASS_OBJECT || ref->nodeClass == UA_NODECLASS_VARIABLE || ref->nodeClass == UA_NODECLASS_METHOD))
            {

                if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
                {
                    printf("%-9d %-16d %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
                           ref->nodeId.nodeId.identifier.numeric, (int)ref->browseName.name.length,
                           ref->browseName.name.data, (int)ref->displayName.text.length,
                           ref->displayName.text.data);
                }
                else if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING)
                {
                    printf("%-9d %-16.*s %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
                           (int)ref->nodeId.nodeId.identifier.string.length,
                           ref->nodeId.nodeId.identifier.string.data,
                           (int)ref->browseName.name.length, ref->browseName.name.data,
                           (int)ref->displayName.text.length, ref->displayName.text.data);
                }
            }
        }
    }

__done:
    UA_BrowseRequest_clear(&browse_req);
    UA_BrowseResponse_clear(&browse_res);
    return retval;
}

UA_StatusCode client_recursive_browse_node(UA_Client *client, UA_NodeId start_id)
{

    UA_StatusCode retval;
    UA_BrowseResponse browse_res;
    UA_BrowseResponse_init(&browse_res);
    UA_BrowseRequest browse_req;
    UA_BrowseRequest_init(&browse_req);
    browse_req.nodesToBrowseSize = 1;// one target
    browse_req.nodesToBrowse = UA_BrowseDescription_new();
    if (browse_req.nodesToBrowse == NULL)
    {
        return UA_STATUSCODE_BADOUTOFMEMORY;
    }
    browse_req.requestedMaxReferencesPerNode = 0;
    browse_req.nodesToBrowse[0].nodeId = start_id;
    browse_req.nodesToBrowse[0].resultMask = UA_BROWSERESULTMASK_ALL;
    browse_res = UA_Client_Service_browse(client, browse_req);
    retval = browse_res.responseHeader.serviceResult;
    if (UA_STATUSCODE_GOOD != retval)
        goto __done;

    for (size_t i = 0; i < browse_res.resultsSize; ++i)
    {   
        for (size_t j = 0; j < browse_res.results[i].referencesSize; ++j)
        {
            UA_ReferenceDescription *ref = &(browse_res.results[i].references[j]);
            if ((ref->nodeClass == UA_NODECLASS_OBJECT || ref->nodeClass == UA_NODECLASS_VARIABLE || ref->nodeClass == UA_NODECLASS_METHOD))
            {

                if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
                {
                    printf("%-9d %-16d %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
                           ref->nodeId.nodeId.identifier.numeric, (int)ref->browseName.name.length,
                           ref->browseName.name.data, (int)ref->displayName.text.length,
                           ref->displayName.text.data);

                    client_recursive_browse_node(client, UA_NODEID_NUMERIC(ref->nodeId.nodeId.namespaceIndex,
                                                                           ref->nodeId.nodeId.identifier.numeric));
                }
                else if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING)
                {
                    printf("%-9d %-16.*s %-16.*s %-16.*s\n", ref->nodeId.nodeId.namespaceIndex,
                           (int)ref->nodeId.nodeId.identifier.string.length,
                           ref->nodeId.nodeId.identifier.string.data,
                           (int)ref->browseName.name.length, ref->browseName.name.data,
                           (int)ref->displayName.text.length, ref->displayName.text.data);
                    client_recursive_browse_node(client, UA_NODEID_STRING(ref->nodeId.nodeId.namespaceIndex,
                                                                          (char *)ref->nodeId.nodeId.identifier.string.data));
                }
            }
        }// todo more:
            UA_BrowseNextRequest browse_next_req;
            UA_BrowseNextRequest_init(&browse_next_req);
            // normally is set to 0, to get all the nodes, but we want to test browse next:refer PART4;
            browse_next_req.releaseContinuationPoints = UA_FALSE;
            browse_next_req.continuationPoints = &browse_res.results[0].continuationPoint; //get remain nums
            browse_next_req.continuationPointsSize = 1;
            UA_BrowseNextResponse browse_next_res = UA_Client_Service_browseNext(client, browse_next_req);
            for (size_t i = 0; i < browse_next_res.resultsSize; i++)
            {
                for (size_t j = 0; j < browse_next_res.results[i].referencesSize; j++)
                {
                    UA_ReferenceDescription *ref = &(browse_next_res.results[i].references[j]);
                    if ((ref->nodeClass == UA_NODECLASS_OBJECT || ref->nodeClass == UA_NODECLASS_VARIABLE || ref->nodeClass == UA_NODECLASS_METHOD))
                    {

                        if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_NUMERIC)
                        {
                            printf("%-9d %-16d %-30.*s %-30.*s\n", ref->nodeId.nodeId.namespaceIndex,
                                ref->nodeId.nodeId.identifier.numeric, (int)ref->browseName.name.length,
                                ref->browseName.name.data, (int)ref->displayName.text.length,
                                ref->displayName.text.data);

                            client_recursive_browse_node(client, UA_NODEID_NUMERIC(ref->nodeId.nodeId.namespaceIndex,
                                                                                ref->nodeId.nodeId.identifier.numeric));
                        }
                        else if (ref->nodeId.nodeId.identifierType == UA_NODEIDTYPE_STRING)
                        {
                            printf("%-9d %-16.*s %-30.*s %-30.*s\n", ref->nodeId.nodeId.namespaceIndex,
                                (int)ref->nodeId.nodeId.identifier.string.length,
                                ref->nodeId.nodeId.identifier.string.data,
                                (int)ref->browseName.name.length, ref->browseName.name.data,
                                (int)ref->displayName.text.length, ref->displayName.text.data);

                            client_recursive_browse_node(client, UA_NODEID_STRING(ref->nodeId.nodeId.namespaceIndex,
                                                                            (char *)ref->nodeId.nodeId.identifier.string.data));
                        }
                    }
                
                }
            }
    }
__done:
    UA_BrowseRequest_clear(&browse_req);
    UA_BrowseResponse_clear(&browse_res);
    return retval;
}

UA_StatusCode client_read_single(UA_Client *client, UA_NodeId node_id, const UA_DataType *data_type, UA_Variant *value)
{
    UA_StatusCode retval;
    UA_Variant *val = UA_Variant_new();
    retval = UA_Client_readValueAttribute(client, node_id, val);
    ASSERT_GOOD(retval);
    if (retval == UA_STATUSCODE_GOOD && UA_Variant_isScalar(val) &&
        val->type == data_type)
    {
       UA_Variant_copy(val,value);
    }
  
    UA_Variant_delete(val);
    return retval;
}

UA_StatusCode client_write_single(UA_Client *client, UA_NodeId node_id, const UA_Variant *value)
{
    UA_StatusCode retval;
    if(NULL ==client || UA_NodeId_isNull(&node_id) || UA_Variant_isEmpty(value))
        return UA_STATUSCODE_BAD;
    retval = UA_Client_writeValueAttribute(client,node_id,value);
    return retval;
}

UA_StatusCode client_read_mutil(UA_Client *client, UA_NodeId node_ids[],UA_Variant values[],size_t nums)
{
    
    UA_StatusCode retval;
    
    if(NULL == client || nums < 1 || NULL == node_ids || NULL == values)
        return UA_STATUSCODE_BAD;
    UA_ReadValueId items[nums];

    for (size_t i = 0; i < nums; ++i)
    {
        UA_ReadValueId_init(&items[i]);
        //read_attribut
        items[i].attributeId = UA_ATTRIBUTEID_VALUE;
        items[i].nodeId = node_ids[i];
    }

    UA_ReadRequest read_req;
    UA_ReadRequest_init(&read_req);
    read_req.nodesToReadSize = nums;
    read_req.nodesToRead = &items[0];
    UA_ReadResponse read_res;
    UA_ReadResponse_init(&read_res);
    read_res = UA_Client_Service_read(client, read_req);
    retval = read_res.responseHeader.serviceResult;
    if(retval != UA_STATUSCODE_GOOD || read_res.resultsSize != nums)
        goto __done;
    
    for(size_t i = 0; i < read_res.resultsSize; ++i)
    {
        if (UA_STATUSCODE_GOOD == read_res.results[i].status)
        {
            UA_DataValue res = read_res.results[i];
            if (!res.hasValue) // no value
            {
                retval =  UA_STATUSCODE_BADUNEXPECTEDERROR;
                goto __done;
            }
            UA_Variant_copy(&res.value, &values[i]);
            //memcpy(&values[i], &res.value, sizeof(UA_Variant));
            UA_Variant_init(&res.value);
        }
        else
        {
            values[i].type = NULL;// UA_Variant_isEmpty()  
        }
    }
__done:
    UA_ReadResponse_clear(&read_res);
    return retval;
}


UA_StatusCode client_write_mutil(UA_Client *client, UA_NodeId node_ids[],  UA_Variant values[],size_t nums)
{
    
    if (NULL == client || nums < 1 || NULL == node_ids || NULL == values)
        return UA_STATUSCODE_BADPARENTNODEIDINVALID;

    UA_StatusCode retval;
    UA_WriteValue write_values[nums];
    for (size_t i = 0; i < nums; ++i)
    {
        UA_WriteValue_init(&write_values[i]);
        write_values[i].attributeId = UA_ATTRIBUTEID_VALUE;
        write_values[i].nodeId = node_ids[i];
        write_values[i].value.value =values[i];
        write_values[i].value.hasValue = UA_TRUE;
    }

    UA_WriteRequest write_req;
    UA_WriteRequest_init(&write_req);
    write_req.nodesToWrite = &write_values[0];
    write_req.nodesToWriteSize = nums;
	UA_WriteResponse write_res;
    UA_WriteResponse_init(&write_res);
    write_res = UA_Client_Service_write(client, write_req);
    retval = write_res.responseHeader.serviceResult;

    UA_WriteRequest_clear(&write_req); 
    UA_WriteResponse_clear(&write_res);
    return retval;
}

