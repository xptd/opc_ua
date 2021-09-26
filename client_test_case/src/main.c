/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 13:38:07
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-24 15:23:55
 */
/*================================================================
*   Copyright (C) 2021 PANDA Ltd. All rights reserved.
*   
*   文件名称：main.c
*   创 建 者：xptd
*   创建日期：2021年09月13日
*   描    述：
*
================================================================*/

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "client.h"
#include "open62541.h"


#define SIMENS_OPC_SERVER "opc.tcp://localhost:4840"




START_TEST(client_connect_test)
{
    UA_StatusCode retval = client_connect(SIMENS_OPC_SERVER);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:connect server err:%s", UA_StatusCode_name(retval));
}
END_TEST

START_TEST(client_get_endpoint_test)
{


    size_t endpoint_nums = 0;
    UA_EndpointDescription* endpoint_descriptors = NULL;

    UA_StatusCode retval = client_get_endpoint(SIMENS_OPC_SERVER, &endpoint_nums,&endpoint_descriptors);
    if (retval != UA_STATUSCODE_GOOD)
    {
        UA_Array_delete(endpoint_descriptors, endpoint_nums, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
    }
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:get endpoint_descriptors err:%s", UA_StatusCode_name(retval));
    fprintf(stdout, "server %s:%i endpoints found\n", SIMENS_OPC_SERVER,(int) endpoint_nums);
    client_dump_endpoints(endpoint_nums, endpoint_descriptors);
    //dont forget
    UA_Array_delete(endpoint_descriptors, endpoint_nums, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
}
END_TEST


START_TEST(client_browse_name2nodeid_single_test)
{
    UA_StatusCode  retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);

    if(retval != UA_STATUSCODE_GOOD)
    {
        UA_Client_disconnect(client);
        UA_Client_delete(client);
        ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:connect server:%s err:%s", SIMENS_OPC_SERVER, UA_StatusCode_name(retval));
    }
    
//test_1
    const char* paths_name[] = {"xptd_integer_node"};
    UA_UInt32 refers_nodeid[] = {UA_NS0ID_ORGANIZES};
    UA_Int32 ns_indexs[] = {1};//opc ua --0; user_local 1:
    UA_NodeId target_nodeid;
    retval = client_browse_name2nodeid_single(client,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
                                              paths_name,
                                              ns_indexs,
                                              refers_nodeid,
                                              sizeof(paths_name) / sizeof(paths_name[0]),
                                              &target_nodeid);
    if(retval != UA_STATUSCODE_GOOD)
    {
        UA_Client_disconnect(client);
        UA_Client_delete(client);
        ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:browse_name2nodeid:%s", UA_StatusCode_name(retval));
    }
    print_nodeid(&target_nodeid);
//test1_end

//test_2
    const char *phone_paths_name[] = {"Objects", "phone", "product_sn"};
    //const char *phone_paths_name[] = {"Objects", "the student", "studeng_name"};
    UA_Int32 phone_ns_indexs[] = {0,1,1};
    UA_UInt32 phone_refers_nodeid[] = {UA_NS0ID_ORGANIZES,UA_NS0ID_ORGANIZES,UA_NS0ID_HASCOMPONENT};
    UA_NodeId_init(&target_nodeid);
    retval = client_browse_name2nodeid_single(client,
                                              UA_NODEID_NUMERIC(0, UA_NS0ID_ROOTFOLDER),
                                              phone_paths_name,
                                              phone_ns_indexs,
                                              phone_refers_nodeid,
                                              sizeof(phone_paths_name) / sizeof(phone_paths_name[0]),
                                              &target_nodeid);
    if(retval != UA_STATUSCODE_GOOD)
    {
        UA_Client_disconnect(client);
        UA_Client_delete(client);
        ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:browse_name2nodeid:%s", UA_StatusCode_name(retval));
    }
   
    print_nodeid(&target_nodeid);
//test2_end

    UA_Client_disconnect(client);
    UA_Client_delete(client);
}
END_TEST



START_TEST(client_simplified_browse_node_test)
{
    UA_StatusCode  retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);
    if(retval != UA_STATUSCODE_GOOD)
       goto __done;

   retval = client_simplified_browse_node(client, UA_NODEID_NUMERIC(0,UA_NS0ID_OBJECTSFOLDER));
   if (retval != UA_STATUSCODE_GOOD)
       goto __done;
__done:
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:%s", UA_StatusCode_name(retval));
}
END_TEST



START_TEST(client_recursive_browse_node_test)
{
    UA_StatusCode retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;

    fprintf(stdout,"%-9s %-16s %-30s %-30s\n", "NAMESPACE", "NODEID", "BROWSE NAME", "DISPLAY NAME");
    retval = client_recursive_browse_node(client, UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER));
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;

   
__done:
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:%s", UA_StatusCode_name(retval));
}
END_TEST

START_TEST(client_read_single_test)
{
    UA_StatusCode retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;

    //UA_Variant value = UA_Variant_new();
    UA_Variant value;
    retval = client_read_single(client, UA_NODEID_NUMERIC(0, 50222), &UA_TYPES[UA_TYPES_STRING], &value);
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;

    UA_String str = *(UA_String *)value.data;
    fprintf(stdout,"client_read_attribute_test:value:%s\n",str.data);
    

__done:
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:%s", UA_StatusCode_name(retval));
}
END_TEST

START_TEST(client_write_single_test)
{
    UA_StatusCode retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;
    UA_Variant *value = UA_Variant_new();
    UA_String test_str = UA_STRING("HELLO WORLD!");
    UA_Variant_setScalarCopy(value, &test_str, &UA_TYPES[UA_TYPES_STRING]);
    retval = client_write_single(client, UA_NODEID_NUMERIC(0, 50222), value);
    UA_Variant_delete(value);//notice;
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;
__done:
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:%s", UA_StatusCode_name(retval));
}
END_TEST




START_TEST(client_write_mutil_test)
{
    UA_StatusCode retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;
    
#define WRITE_NUMS 2
    //name //index
    UA_NodeId node_ids[WRITE_NUMS] = {UA_NODEID_NUMERIC(0, 50222), UA_NODEID_NUMERIC(0, 50223)};

    UA_Variant *obj_values = UA_Array_new(WRITE_NUMS, &UA_TYPES[UA_TYPES_VARIANT]);
    if(NULL == obj_values)
    {
        retval = UA_STATUSCODE_BADOUTOFMEMORY;
        goto __done;
    }
    //name
    UA_String name_str = UA_STRING("PANDA ROBOT");
    //name
    UA_UInt64 indxe = 12345;
    UA_Variant_setScalarCopy(&obj_values[0],&name_str,&UA_TYPES[UA_TYPES_STRING]);
    UA_Variant_setScalarCopy(&obj_values[1],&indxe,&UA_TYPES[UA_TYPES_UINT64]);
    retval = client_write_mutil(client, node_ids, obj_values, WRITE_NUMS);

    UA_Array_delete(obj_values, WRITE_NUMS, &UA_TYPES[UA_TYPES_VARIANT]);
__done:
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:%s", UA_StatusCode_name(retval));
}
END_TEST



START_TEST(client_read_mutil_test)
{
    UA_StatusCode retval;
    UA_Client *client = UA_Client_new();
    UA_ClientConfig_setDefault(UA_Client_getConfig(client));
    retval = UA_Client_connect(client, SIMENS_OPC_SERVER);
    if (retval != UA_STATUSCODE_GOOD)
        goto __done;
    //client_read_mutil(UA_Client *client, UA_NodeId node_ids[],UA_Variant values[],size_t nums)

#define NUMS 4
    UA_NodeId node_ids[NUMS] = {UA_NODEID_NUMERIC(0,50224),
                                UA_NODEID_NUMERIC(0,50223),
                                UA_NODEID_NUMERIC(0,50222),
                                UA_NODEID_NUMERIC(0,50225)};

    UA_Variant *obj_values = UA_Array_new(NUMS,&UA_TYPES[UA_TYPES_VARIANT]);
    if(NULL == obj_values)
    {
        retval = UA_STATUSCODE_BADOUTOFMEMORY;
        goto __done;
    }
    retval = client_read_mutil(client,node_ids,obj_values,NUMS);
    if(retval != UA_STATUSCODE_GOOD)
        goto __clear;

   const UA_DataType *obj_types[NUMS]={&UA_TYPES[UA_TYPES_BOOLEAN],&UA_TYPES[UA_TYPES_UINT64],&UA_TYPES[UA_TYPES_STRING],&UA_TYPES[UA_TYPES_DATETIME]};
    for(size_t i = 0; i < NUMS;++i)
    {        
        if (UA_Variant_isEmpty(&obj_values[i]) || obj_values[i].type != obj_types[i])
        {
            fprintf(stdout,"get err\n");
            continue;
        }
    }
//name
    UA_String str = *(UA_String *)obj_values[2].data;
    fprintf(stdout,"name:%.*s\n",(int)str.length,str.data);
//index
    UA_UInt64 indxe = *(UA_UInt64 *)obj_values[1].data;
    fprintf(stdout, "index:%ld\n", indxe);

__clear :
    UA_Array_delete(obj_values, NUMS, &UA_TYPES[UA_TYPES_VARIANT]);
__done:
    UA_Client_disconnect(client);
    UA_Client_delete(client);
    ck_assert_msg(retval == UA_STATUSCODE_GOOD, "ERROR:%s", UA_StatusCode_name(retval));
}
END_TEST


Suite *set_up_client_test(void)
{
    Suite *test_suit = suite_create("xptd test suit for opc_client");
    TCase *test_case_1 = tcase_create("opc ua client tcase");

//add test list
    //client_connect_test
    tcase_add_test(test_case_1, client_connect_test);
    //client_get_endpoint_test
    tcase_add_test(test_case_1, client_get_endpoint_test);
    //client_browse_name2nodeid_single_test
    tcase_add_test(test_case_1, client_browse_name2nodeid_single_test);
    //client_simple_browse_node_test
    tcase_add_test(test_case_1, client_simplified_browse_node_test);
    //client_recursive_browse_node_test
    tcase_add_test(test_case_1, client_read_single_test);
    //client_read_attribute_test
    tcase_add_test(test_case_1, client_write_single_test);
    //client_write_attribute_test
    tcase_add_test(test_case_1, client_write_single_test);
    //client_write_mutil_test
    tcase_add_test(test_case_1, client_write_mutil_test);
    //client_read_mutil_test
    tcase_add_test(test_case_1, client_read_mutil_test);

//test list end
    suite_add_tcase(test_suit, test_case_1);
    return test_suit;
}



//SRunner-->Suite-->TCase-->test_case(START_TEST...END_TEST);
int main(int argc, char *argv)
{
    fprintf(stdout, "opc client test file");
    int retval;
    SRunner *test_suit_runner;
    test_suit_runner = srunner_create(set_up_client_test());
    srunner_run_all(test_suit_runner, CK_NORMAL);
    retval = srunner_ntests_failed(test_suit_runner);
    srunner_free(test_suit_runner);

    return retval == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}