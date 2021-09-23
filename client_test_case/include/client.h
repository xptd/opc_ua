/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 11:22:01
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-23 13:54:24
 */

#pragma once

#ifndef CLIENT_H_
#define CLIENT_H_H

#include "open62541.h"

#define ASSERT_GOOD(X)           \
    if (X != UA_STATUSCODE_GOOD) \
        return X;

/**
 * @author: xptd
 * @brief: 
 * @input: 
 * @output: 
 * @return {*}
 * @others: 
 */


UA_StatusCode client_connect(const char *endpoint_url);
/**
 * @author: xptd
 * @brief: 
 * @input: 
 *  server_url
 *  
 * @output: 
 *  size_t* nums_endpoints:
 *  UA_EndpointDescription **endpoint_des(endpointDescriptions)
 * @return {*}
 * @others: Every Server has one or more DiscoveryUrls that allow access to its Endpoints. Once a Client
            obtains (e.g. via manual entry into a form) the DiscoveryUrl for the Server, it reads the
            EndpointDescriptions using the GetEndpoints Service defined in Part 4.
 */

UA_StatusCode client_get_endpoint(const char *server_url, size_t *endpoint_nums, UA_EndpointDescription **endpoint_des);
/**
 * @author: xptd
 * @brief: 
 * @input: 
 *  endpoint_nums
 *  UA_EndpointDescription *endpoint_des
 * @output: 
 * @return {*}
 * @others: 
 * @param {size_t} endpoint_nums
 * @param {UA_EndpointDescription} *endpoint_des
 */
void client_dump_endpoints(size_t endpoint_nums, UA_EndpointDescription *endpoint_des);

/**
 * @author: xptd
 * @brief: 
 * @input: 
 *  start_nodeid:'root' nodeid
 *  paths_browsename[]:browsnames([]:array )
 *  indexs[]:namespace index
 *  refer_ids[]:source ---reference---target
 *  path_nums:path tiers /A/B/C/D
 * @output:     
 *  target_nodeid: browsename to nodeid
 * @return {*}
 * @others: 
 */

UA_StatusCode client_browse_name2nodeid_single(UA_Client *client, UA_NodeId start_nodeid, const char* paths_browsename[], UA_Int32 indexs[],UA_UInt32 refer_ids[],size_t path_nums,UA_NodeId *target_nodeid);
/**
 * @author: xptd
 * @brief: 
 * @input: 
 *  target nodeid(pointer)
 * @output: 
 * @return {*}
 * @others: 
 */
void print_nodeid(const UA_NodeId*);


#endif