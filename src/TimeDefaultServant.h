
#ifndef __TIME_DEFAULT_SERVANT_H
#define __TIME_DEFAULT_SERVANT_H

#ident "$Id$"

#include "timeS.h"

// Server code assumes that servants will use reference counting
// [Henning1999 p492]
class TimeDefaultServant : public virtual POA_Time, 
             public virtual PortableServer::RefCountServantBase 
{
 public:
  TimeDefaultServant (PortableServer::Current_ptr current);
  virtual ~TimeDefaultServant (void);
  virtual TimeOfDay get_time () throw (CORBA::SystemException);

 private:
  char *getObjectId ();

  // copy not supported
  TimeDefaultServant (const TimeDefaultServant & t);
  void operator = (const TimeDefaultServant & t);

  PortableServer::Current_var _current;
};

#endif // __TIME_DEFAULT_SERVANT_H
