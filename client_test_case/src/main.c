/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 13:38:07
 * @LastEditors: xptd
 * @LastEditTime: 2021-09-22 14:11:50
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
    clien_dump_endpoints(endpoint_nums, endpoint_descriptors);
    //dont forget
    UA_Array_delete(endpoint_descriptors, endpoint_nums, &UA_TYPES[UA_TYPES_ENDPOINTDESCRIPTION]);
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