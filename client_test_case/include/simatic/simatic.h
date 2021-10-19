/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-09-24 16:16:16
 * @LastEditors: xptd
 * @LastEditTime: 2021-10-19 14:41:04
 */

#pragma once

#ifndef SIMATIC_H_
#define SIMATIC_H_


#include "client/client.h"

#define SIMENS_OPC_SERVER "opc.tcp://"
/**
 * @author: xptd
 * @brief: 
 * @input: 
 * @output: 
 * @return {*}
 *  0：successed
*  -1：failed
 * @others: 
 */
int connect_simatic_server(const char*);
#endif