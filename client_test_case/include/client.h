/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 11:22:01
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-22 14:15:03
 */

#pragma once

#ifndef CLIENT_H_
#define CLIENT_H_H

#include "open62541.h"
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
void clien_dump_endpoints(size_t endpoint_nums, UA_EndpointDescription *endpoint_des);

#endif