/*================================================================
*   Copyright (C) 2021 PANDA Ltd. All rights reserved.
*   
*   文件名称：client.h
*   创 建 者：xptd
*   创建日期：2021年09月13日
*   描    述：
*
================================================================*/


#pragma once

#ifndef CLIENT_H_
#define CLIENT_H_H

#include "open62541.h"
UA_StatusCode client_connect(const char* );
#endif