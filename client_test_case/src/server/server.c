/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-22 14:20:49
 * @LastEditors: xptd
 * @LastEditTime: 2021-10-19 14:41:40
 */
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "client/client.h"
#include "opc/open62541.h"

typedef struct 
{
    UA_String name;
    UA_UInt64 index;
    UA_DateTime sn;
    UA_Boolean is_check;

} Product, *pProduct;

static UA_Boolean server_running = UA_TRUE;

static void sig_handler()
{
    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_SERVER,"server terminated");
    server_running = UA_FALSE;
}

static UA_StatusCode get_componet_id(UA_Server *server,
                                     UA_NodeId parent_id,
                                     const int path_cnt,
                                     const UA_QualifiedName *target_name,
                                     UA_NodeId *target_id)
{
    UA_StatusCode retval;
    UA_BrowsePathResult bpr = UA_Server_browseSimplifiedBrowsePath(server,
                                                                   parent_id, path_cnt, target_name);
    retval = bpr.statusCode;
    if (UA_STATUSCODE_GOOD == retval && (bpr.targetsSize >= 1))
    {
        UA_NodeId_copy(&bpr.targets[0].targetId.nodeId, target_id);
    }
    UA_BrowsePathResult_clear(&bpr);
    return retval;
}

UA_StatusCode add_integer_node(
    UA_Server* server,
    UA_NodeId parent_nodeid,
    UA_NodeId refer_nodeid,
    UA_NodeId *target_nodeid,
    const char* node_name,
    const char* description,
    const char* display_name
)
{
        UA_VariableAttributes v_attr = UA_VariableAttributes_default;
        v_attr.displayName = UA_LOCALIZEDTEXT("en-US", (char *)display_name);
        v_attr.description = UA_LOCALIZEDTEXT("en-US",(char *)description);
        v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
        v_attr.dataType = UA_TYPES[UA_TYPES_INT32].typeId;
        v_attr.valueRank = UA_VALUERANK_SCALAR;

        UA_Int32 init_value = 10;
        UA_Variant_setScalar(&(v_attr.value), &init_value, &UA_TYPES[UA_TYPES_INT32]);

        return UA_Server_addVariableNode(
            server,
            UA_NODEID_NULL,
            parent_nodeid,
            refer_nodeid,
            UA_QUALIFIEDNAME(1, (char *)node_name),
            UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
            v_attr,
            NULL,
            target_nodeid);
}

//UA_String name
//UA_UInt32 age
UA_StatusCode add_sdtudent_object(UA_Server *server, UA_NodeId *obj_nodeid)
{

    UA_StatusCode retval ;
    UA_ObjectAttributes obj_attr  = UA_ObjectAttributes_default;
    obj_attr.displayName = UA_LOCALIZEDTEXT("en-US","student");
    obj_attr.description = UA_LOCALIZEDTEXT("en-US","add a studengt object node for test");

    retval = UA_Server_addObjectNode(
        server,
        UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0,UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0,UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1,"the student"),
        UA_NODEID_NUMERIC(0,UA_NS0ID_BASEOBJECTTYPE),
        obj_attr,
        NULL,
        obj_nodeid
        );

    if(retval != UA_STATUSCODE_GOOD || UA_NodeId_isNull(obj_nodeid))
    {
       return UA_STATUSCODE_BAD;
    }
//add name 
    UA_NodeId nodeid;
    UA_VariableAttributes v_attr = UA_VariableAttributes_default;
    v_attr.displayName = UA_LOCALIZEDTEXT("en-US","name");
    v_attr.description = UA_LOCALIZEDTEXT("en-US", "name of student");
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    v_attr.dataType =UA_TYPES[UA_TYPES_STRING].typeId; 
    v_attr.valueRank = UA_VALUERANK_SCALAR;
    UA_String stu_name = UA_STRING("xptd");
    UA_Variant_setScalar(&v_attr.value,&stu_name,&UA_TYPES[UA_TYPES_STRING]);
    retval = UA_Server_addVariableNode(
        server,
        UA_NODEID_NULL,
        *obj_nodeid,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "studeng_name"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        v_attr,
        NULL,
        &nodeid);
    ASSERT_GOOD(retval);
    print_nodeid((const UA_NodeId *)&nodeid);

//add age
    v_attr = UA_VariableAttributes_default;
    v_attr.displayName = UA_LOCALIZEDTEXT("en-US","age");
    v_attr.description = UA_LOCALIZEDTEXT("en-US", "age of student");
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    v_attr.dataType = UA_TYPES[UA_TYPES_UINT32].typeId;
    v_attr.valueRank = UA_VALUERANK_SCALAR;
    UA_UInt32 stu_age = 20;
    UA_Variant_setScalar(&v_attr.value,&stu_age,&UA_TYPES[UA_TYPES_UINT32]);
    UA_NodeId_init(&nodeid);

    retval = UA_Server_addVariableNode(
        server,
        UA_NODEID_NULL,
        *obj_nodeid,
        UA_NODEID_NUMERIC(0,UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1,"studeng_age"),
        UA_NODEID_NUMERIC(0,UA_NS0ID_BASEDATAVARIABLETYPE),
        v_attr,
        NULL,
        &nodeid
    );

    ASSERT_GOOD(retval);
    print_nodeid((const UA_NodeId *)&nodeid);
    return retval;
}


//name  :UA_String
//index :UA_UInt64
//SN:UA_Date
//check :UA_Boolean
UA_StatusCode add_product_type(UA_Server *server,UA_NodeId *p_type_id)
{

    UA_StatusCode retval;
    UA_ObjectTypeAttributes t_attr = UA_ObjectTypeAttributes_default;
    t_attr.displayName = UA_LOCALIZEDTEXT("en-US","Product");
    t_attr.description  = UA_LOCALIZEDTEXT("en-US","add a product objecy type node for test");
    retval = UA_Server_addObjectTypeNode(
        server,
        UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEOBJECTTYPE),
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASSUBTYPE),
        UA_QUALIFIEDNAME(1,"Product Type"),
        t_attr,
        NULL,
        p_type_id 
        );
    ASSERT_GOOD(retval);

//add name
    UA_NodeId nodeid;
    UA_VariableAttributes v_attr = UA_VariableAttributes_default;
    v_attr.displayName = UA_LOCALIZEDTEXT("en-US", "name");
    v_attr.description = UA_LOCALIZEDTEXT("en-US", "name of product");
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    v_attr.dataType = UA_TYPES[UA_TYPES_STRING].typeId;
    v_attr.valueRank = UA_VALUERANK_SCALAR;
    UA_String p_name = UA_STRING("xptd");
    UA_Variant_setScalar(&v_attr.value, &p_name, &UA_TYPES[UA_TYPES_STRING]);
    retval = UA_Server_addVariableNode(
        server,
        UA_NODEID_NULL,
        *p_type_id,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "product_name"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        v_attr,
        NULL,
        &nodeid);
    ASSERT_GOOD(retval);
    print_nodeid((const UA_NodeId *)&nodeid);
    UA_Server_addReference(server, nodeid,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);

    //add index
    v_attr = UA_VariableAttributes_default;
    v_attr.displayName = UA_LOCALIZEDTEXT("en-US","index");
    v_attr.description = UA_LOCALIZEDTEXT("en-US", "index of student");
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    v_attr.dataType = UA_TYPES[UA_TYPES_UINT64].typeId;
    v_attr.valueRank = UA_VALUERANK_SCALAR;
    UA_UInt64 p_index = 1;
    UA_Variant_setScalar(&v_attr.value, &p_index, &UA_TYPES[UA_TYPES_UINT64]);
    UA_NodeId_init(&nodeid);

    retval = UA_Server_addVariableNode(
        server,
        UA_NODEID_NULL,
        *p_type_id,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "product_index"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        v_attr,
        NULL,
        &nodeid);

    ASSERT_GOOD(retval);
    print_nodeid((const UA_NodeId *)&nodeid);
    UA_Server_addReference(server, nodeid,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
    //add check
    v_attr = UA_VariableAttributes_default;
    v_attr.displayName = UA_LOCALIZEDTEXT("en-US","check");
    v_attr.description = UA_LOCALIZEDTEXT("en-US", "check of student");
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    v_attr.dataType = UA_TYPES[UA_TYPES_BOOLEAN].typeId;
    v_attr.valueRank = UA_VALUERANK_SCALAR;
    UA_Boolean is_check = UA_FALSE;
    UA_Variant_setScalar(&v_attr.value, &is_check, &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_NodeId_init(&nodeid);

    retval = UA_Server_addVariableNode(
        server,
        UA_NODEID_NULL,
        *p_type_id,
        UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
        UA_QUALIFIEDNAME(1, "product_check"),
        UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
        v_attr,
        NULL,
        &nodeid);

    ASSERT_GOOD(retval);
    print_nodeid((const UA_NodeId *)&nodeid);
    UA_Server_addReference(server, nodeid,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
   
//add SN
    v_attr = UA_VariableAttributes_default;
    v_attr.displayName = UA_LOCALIZEDTEXT("en-US", "sn");
    v_attr.description = UA_LOCALIZEDTEXT("en-US", "sn of student");
    v_attr.accessLevel = UA_ACCESSLEVELMASK_READ | UA_ACCESSLEVELMASK_WRITE;
    v_attr.dataType = UA_TYPES[UA_TYPES_DATETIME].typeId;
    v_attr.valueRank = UA_VALUERANK_SCALAR;
    UA_DateTime now = UA_DateTime_now();

    UA_Variant_setScalar(&v_attr.value, &now, &UA_TYPES[UA_TYPES_DATETIME]);
    UA_NodeId_init(&nodeid);

     retval = UA_Server_addVariableNode(
         server,
         UA_NODEID_NULL,
         *p_type_id,
         UA_NODEID_NUMERIC(0, UA_NS0ID_HASCOMPONENT),
         UA_QUALIFIEDNAME(1, "product_sn"),
         UA_NODEID_NUMERIC(0, UA_NS0ID_BASEDATAVARIABLETYPE),
         v_attr,
         NULL,
         &nodeid);

     ASSERT_GOOD(retval);
     print_nodeid((const UA_NodeId *)&nodeid);

     // default: option;
     UA_Server_addReference(server, nodeid,
                           UA_NODEID_NUMERIC(0, UA_NS0ID_HASMODELLINGRULE),
                           UA_EXPANDEDNODEID_NUMERIC(0, UA_NS0ID_MODELLINGRULE_MANDATORY), true);
     return retval;
}


UA_StatusCode add_product_obj_defalut(UA_Server *server, const UA_NodeId *type_id, UA_NodeId *target_id,const char *display_name)
{

    UA_ObjectAttributes obj_attr = UA_ObjectAttributes_default;
    obj_attr.description = UA_LOCALIZEDTEXT("en-US", "create Product Object Instance");
    obj_attr.displayName = UA_LOCALIZEDTEXT("en-US", (char*)display_name);
    return UA_Server_addObjectNode(
        server,
        UA_NODEID_NULL,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        UA_QUALIFIEDNAME(1, "phone"),
        *type_id,
        obj_attr,
        NULL,
        target_id
        );
}


UA_StatusCode
add_product_obj(UA_Server *server, const UA_NodeId *type_id, UA_NodeId *target_id,const char* display_name,pProduct info)
{
    UA_StatusCode retval;
    UA_NodeId obj_id;
    UA_NodeId component_id;
    UA_Variant tmp_value;
    UA_QualifiedName componet_name;
    retval = add_product_obj_defalut(server, type_id, target_id, display_name);
    ASSERT_GOOD(retval);
    if(UA_NodeId_isNull(target_id))
    {
        return UA_STATUSCODE_BAD;
    }
// name
    UA_Variant_init(&tmp_value);
    UA_Variant_setScalar(&tmp_value, &(info->name),&UA_TYPES[UA_TYPES_STRING]);
    UA_QualifiedName_init(&componet_name);
    componet_name = UA_QUALIFIEDNAME(1, "product_name");
    retval = get_componet_id(server,*target_id,1,&componet_name,&component_id);
    ASSERT_GOOD(retval);
    if (UA_NodeId_isNull(&component_id))
    {
        return UA_STATUSCODE_BAD;
    }
    retval = UA_Server_writeValue(server, component_id,tmp_value);
    ASSERT_GOOD(retval);
//index
    UA_Variant_init(&tmp_value);
    UA_Variant_setScalar(&tmp_value, &(info->index), &UA_TYPES[UA_TYPES_UINT64]);
    UA_QualifiedName_init(&componet_name);
    componet_name = UA_QUALIFIEDNAME(1, "product_index");
    retval = get_componet_id(server, *target_id, 1, &componet_name, &component_id);
    ASSERT_GOOD(retval);
    if (UA_NodeId_isNull(&component_id))
    {
        return UA_STATUSCODE_BAD;
    }
    retval = UA_Server_writeValue(server, component_id, tmp_value);
    ASSERT_GOOD(retval);
//sn
    UA_Variant_init(&tmp_value);
    UA_Variant_setScalar(&tmp_value, &(info->sn), &UA_TYPES[UA_TYPES_DATETIME]);
    UA_QualifiedName_init(&componet_name);
    componet_name = UA_QUALIFIEDNAME(1, "product_sn");
    retval = get_componet_id(server, *target_id, 1, &componet_name, &component_id);
    ASSERT_GOOD(retval);
    if (UA_NodeId_isNull(&component_id))
    {
        return UA_STATUSCODE_BAD;
    }
    retval = UA_Server_writeValue(server, component_id, tmp_value);
    ASSERT_GOOD(retval);
//is_check
    UA_Variant_init(&tmp_value);
    UA_Variant_setScalar(&tmp_value, &(info->is_check), &UA_TYPES[UA_TYPES_BOOLEAN]);
    UA_QualifiedName_init(&componet_name);
    componet_name = UA_QUALIFIEDNAME(1, "product_check");
    retval = get_componet_id(server, *target_id, 1, &componet_name, &component_id);
    ASSERT_GOOD(retval);
    if (UA_NodeId_isNull(&component_id))
    {
        return UA_STATUSCODE_BAD;
    }
    retval = UA_Server_writeValue(server, component_id, tmp_value);
    ASSERT_GOOD(retval);

    return retval;
}

int main(int argc, char *argv)
{

    signal(SIGINT,sig_handler);
    signal(SIGTERM,sig_handler);
    UA_LOG_INFO(UA_Log_Stdout,UA_LOGCATEGORY_SERVER,"hello xptd for opc_ua_server test");
    UA_StatusCode retval;
    UA_Server *server= UA_Server_new();
    UA_ServerConfig_setDefault(UA_Server_getConfig(server));

// add a integer variable node
    UA_NodeId integer_nodeid;
    retval = add_integer_node(
        server,
        UA_NODEID_NUMERIC(0, UA_NS0ID_OBJECTSFOLDER),
        UA_NODEID_NUMERIC(0, UA_NS0ID_ORGANIZES),
        &integer_nodeid,
        (char *)"xptd_integer_node",
        (char *)"add a integer variable node for test",
        (char *)"xptd_interger");

    if(retval != UA_STATUSCODE_GOOD || UA_NodeId_isNull(&integer_nodeid))
    {
        UA_LOG_ERROR(UA_Log_Stdout,UA_LOGCATEGORY_USERLAND,"add integer variable node err");
    }
    else
    {
        print_nodeid((const UA_NodeId *)&integer_nodeid);
    }   
// add a integer variable end

// add student object
    UA_NodeId  obj_nodeid;
    retval = add_sdtudent_object(server,  &obj_nodeid);
    if((retval == UA_STATUSCODE_GOOD) && (!UA_NodeId_isNull(&obj_nodeid)))
    {
        print_nodeid((const UA_NodeId *)&obj_nodeid);
    }
//add student objetc end

//add product object type
    UA_NodeId  obj_type_id;
    retval = add_product_type(server,&obj_type_id);

    if((retval != UA_STATUSCODE_GOOD) || (UA_NodeId_isNull(&obj_nodeid)))
    {
        server_running = UA_TRUE;
        goto __done;
    }

    print_nodeid((const UA_NodeId *)&obj_type_id);
//add product object type end

// add product object defalut
    UA_NodeId  phone_id;
    retval = add_product_obj_defalut(server,(const UA_NodeId *)&obj_type_id,  &phone_id,"phone");
    if ((retval != UA_STATUSCODE_GOOD) || (UA_NodeId_isNull(&obj_nodeid)))
    {
        server_running = UA_TRUE;
        goto __done;
    }
    print_nodeid((const UA_NodeId *)&obj_nodeid);
    //create end

    // create product object instance
    UA_NodeId  pad_id;
    Product pad_info={UA_STRING("ipad"),100,UA_DateTime_now(),UA_FALSE};
    retval = add_product_obj(server, (const UA_NodeId *)&obj_type_id, &pad_id, "pad", &pad_info);
    if ((retval != UA_STATUSCODE_GOOD) || (UA_NodeId_isNull(&pad_id)))
    {
        server_running = UA_TRUE;
        goto __done;
    }
    print_nodeid((const UA_NodeId *)&pad_id);
    //create end

__done : 
    retval = UA_Server_run(server, &server_running);
    UA_Server_delete(server);
    return retval == UA_STATUSCODE_GOOD ? EXIT_FAILURE : EXIT_SUCCESS;
}
