/*
 * @Descripttion: 
 * @version: v0.1
 * @Author: xptd
 * @Date: 2021-10-19 15:22:02
 * @LastEditors: xptd
 * @LastEditTime: 2021-10-20 13:34:30
 */
#include <stdio.h>
#include "log/logger.h"


zlog_category_t *glogger;

const char *log_category_names[7] = {"network", "channel", "session", "server",
                                   "client", "userland", "securitypolicy"};

void log_out(void *context, UA_LogLevel level, UA_LogCategory category,
                     const char *msg, va_list args)
{
    if (context != NULL && (UA_LogLevel)(uintptr_t)context > level)
    {
        return;
    }
    const uint32_t LOGBUFSIZE = 512;
    char logbuf[LOGBUFSIZE];

    int pos = snprintf(logbuf, LOGBUFSIZE, "[%s] ", log_category_names[category]);
    if (pos < 0)
    {
        zlog_warn(glogger, "%s", "Log message too long for syslog");
        return;
    }

    pos = vsnprintf(&logbuf[pos], LOGBUFSIZE - (size_t)pos, msg, args);
    if (pos < 0)
    {
        zlog_warn(glogger, "%s", "Log message too long for syslog");
        return;
    }
    switch (level) 
    {
        case UA_LOGLEVEL_DEBUG:
            zlog_debug(glogger,"%s",logbuf);
            break;
        case UA_LOGLEVEL_INFO:
            zlog_info(glogger,"%s",logbuf);
            break;
        case UA_LOGLEVEL_WARNING:
            zlog_warn(glogger, "%s", logbuf);
            break;
        case UA_LOGLEVEL_ERROR:
            zlog_error(glogger,"%s",logbuf);
            break;
        case UA_LOGLEVEL_FATAL:
            zlog_fatal(glogger,"%s",logbuf);
            break;
        case UA_LOGLEVEL_TRACE:
        default:
            return;
    }
}

void log_clear(void *logContext)
{
    /* closelog is optional. We don't use it as several loggers might be
     * instantiated in parallel. 
     */
    fprintf(stderr,"clear zlog\n");
    zlog_fini();
    /* closelog(); */
}


int log_init(const char *config_file)
{
    int ret;
    ret = zlog_init(config_file);
    if(ret)
    {
        fprintf(stderr, "open %s config file failed\n", config_file);
        return -1;
    }
    glogger = zlog_get_category("xptd");
    if (!glogger) 
    {
        fprintf(stderr, "get cat fail\n");
        zlog_fini();
        return -2;
    }
    return 0;
}


