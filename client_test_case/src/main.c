/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 13:38:07
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-23 16:21:22
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