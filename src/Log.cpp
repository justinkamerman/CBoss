
#ident "$Id$"

#include "Log.h"


void  debug (const char *fmt, ...)
{
  va_list ap;
  va_start (ap, fmt);
  char message[MAX_LOG_MESSAGE_LEN + 1]; 
    
  vsnprintf (message, sizeof(message), fmt, ap);
  ACE_DEBUG ((LM_DEBUG, "%D %n(%P|%t): %s\n", message)); 
}


void  error (const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  char message[MAX_LOG_MESSAGE_LEN + 1]; 
    
  vsnprintf(message, sizeof(message), fmt, ap);
  ACE_ERROR ((LM_DEBUG, "%D %n(%P|%t): %s\n", message)); 
}

