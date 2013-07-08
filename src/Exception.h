
#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#ident "$Id$"

#include <iostream>
#include <stdarg.h>

#define MAX_MESSAGE_LENGTH 512
#define MAX_FILENAME_LENGTH 80


class Exception 
{
 public:
  Exception ();
  Exception (const char *fmt, ...);
  Exception (const std::string message);
  Exception (const char *file, const int line, const char *fmt, ...);
  Exception (const char *file, const int line, const std::string message);

  virtual ~Exception () {};

  const int getLine (void) const { return _line; };
  const char *getFile (void) const { return _file; };
  const char *getMessage (void) const { return _message; };

  int getLine (void) { return _line; };
  char *getFile (void) { return _file; };
  char *getMessage (void) { return _message; };
      
 private:
  int _line;
  char _file[MAX_FILENAME_LENGTH + 1];
  char _message[MAX_MESSAGE_LENGTH + 1];
};

std::ostream &operator<<(std::ostream &stream, Exception exp);

#endif // __EXCEPTION_H
