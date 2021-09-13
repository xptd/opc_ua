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
Suite *set_up_client_test(void)
{
    Suite *test_suit = suite_create("xptd test suit for opc_client");
    TCase *test_case_1 = tcase_create("opc ua client tcase");
    tcase_add_test(test_case_1, client_connect_test);
    //to add
    suite_add_tcase(test_suit, test_case_1);
    return test_suit;
}
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