/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-10-19 15:02:51
 * @LastEditors: xptd
 * @LastEditTime: 2021-10-20 13:29:59
 */

#ifndef __LOGGER_H_
#define __LOGGER_H_

#include "opc/open62541.h"
#include "log/zlog.h"

#define LOG_CONFIG_FILE "zlog.conf"
/**
 * @author: xptd
 * @brief: 
 * @input: 
 * @output: 
 * @return {*}
 * @others: 
 */
void log_out(void *context, UA_LogLevel level, UA_LogCategory category,
                     const char *msg, va_list args);
/**
 * @author: xptd
 * @brief: 
 * @input: 
 * @output: 
 * @return {*}
 * @others: 
 * @param {void} *logContext
 */
void log_clear(void *logContext);

/**
 * @author: xptd
 * @brief: 
 * @input: 
 * @output: 
 * @return {*}
 *  0:init success
 *  other: init failed
 * @others: 
 * @param {char*} config_file
 */
int log_init(const char* config_file);


#endif