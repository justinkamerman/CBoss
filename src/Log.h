
#ifndef __LOG_H
#define __LOG_H

#ident "$Id$"

#include <stdarg.h>
#include "ace/Log_Msg.h"


const int MAX_LOG_MESSAGE_LEN = 200;

void  debug(const char *fmt, ...);
void  error(const char *fmt, ...);
                                                          
#endif // __LOG_H
