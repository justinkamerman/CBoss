
#ifndef __TIME_ZONE_REGISTRY_H
#define __TIME_ZONE_REGISTRY_H

#ident "$Id$"

#include <map>
#include <pthread.h>

// [Stroustrup1997 17.1.4.1] 
struct Cstring_less
{
  bool operator () (const char *p, const char *q) const 
  {
    return strcmp (p, q) < 0;
  }
};

typedef std::map <char *, int, Cstring_less> TimeZoneMap;

// This class is a testing artifact. It is a place-holder
// for a thread-safe database which servants query for 
// state information
class TimeZoneRegistry
{
 public:
  static int getOffset (char *location);
  virtual ~TimeZoneRegistry () {};

 protected:
  TimeZoneRegistry () {};
      
 private:
  TimeZoneRegistry (const TimeZoneRegistry &);
  TimeZoneRegistry operator = (const TimeZoneRegistry &);
  static void initialize (void);

  static pthread_mutex_t mutex;
  static bool initialized;
  static TimeZoneMap _timeZoneMap;
};

#endif // __TIME_ZONE_REGISTRY_H
