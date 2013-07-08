
#ident "$Id$"

#include <stdio.h>
#include <string.h>
#include "Exception.h"


Exception::Exception (void)
{
  _line = 0;
  memset (_file, '\0', sizeof (_file));
  memset (_message, '\0', sizeof (_message));
}
      
Exception::Exception (const char *fmt, ...) 
{ 
  _line = 0;
  memset (_file, '\0', sizeof (_file));

  va_list ap;
  va_start (ap, fmt);
  vsnprintf (_message, sizeof(_message), fmt, ap);
}

Exception::Exception (const std::string message)
{
  _line = 0;
  memset (_file, '\0', sizeof (_file));
  strncpy (_message, message.c_str(), MAX_MESSAGE_LENGTH);
}

Exception::Exception (const char *file, 
                      const int line, 
                      const char *fmt, ...)
{
  _line = line;
  strncpy (_file, file, sizeof(_file));

  va_list ap;
  va_start (ap, fmt);
  vsnprintf (_message, sizeof(_message), fmt, ap);
}
  
Exception::Exception (const char *file, 
                      const int line, 
                      const std::string message)
{
  _line = line;
  strncpy (_file, file, MAX_FILENAME_LENGTH);
  strncpy (_message, message.c_str(), MAX_MESSAGE_LENGTH);
}


std::ostream &operator<<(std::ostream &stream, Exception exp)
{
  stream << exp.getFile () << ":";
  stream << exp.getLine () << ": ";
  stream << exp.getMessage ();
  
  return stream;
}
