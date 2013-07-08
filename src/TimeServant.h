
#ifndef __TIME_SERVANT_H
#define __TIME_SERVANT_H

#ident "$Id$"

#include "timeS.h"

// Server code assumes that servants will use reference counting
// [Henning1999 p492]
class TimeServant : public virtual POA_Time, 
             public virtual PortableServer::RefCountServantBase 
{
 public:
  TimeServant (const char *location);
  virtual ~TimeServant (void);
  virtual TimeOfDay get_time () throw (CORBA::SystemException);

 private:

  // copy not supported
  TimeServant (const TimeServant & t);
  void operator = (const TimeServant & t);

  char *_location;
  int _offset;
};

#endif // __TIME_SERVANT_H
