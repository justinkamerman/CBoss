
#ifndef __TIME_SERVANT_ACTIVATOR_H
#define __TIME_SERVANT_ACTIVATOR_H

#ident "$Id$"

#include "tao/PortableServer/PortableServer.h"

class TimeServantActivator : public virtual PortableServer::ServantActivator
{
 public:
  TimeServantActivator (void);
  virtual ~TimeServantActivator (void);

  virtual PortableServer::Servant incarnate 
    (const PortableServer::ObjectId & oid, 
     PortableServer::POA_ptr poa)
    throw (CORBA::SystemException , PortableServer::ForwardRequest);
   
  virtual void etherealize 
    (const PortableServer::ObjectId & oid, 
     PortableServer::POA_ptr poa,
     PortableServer::Servant servant,
     CORBA::Boolean cleanup_in_progress,
     CORBA::Boolean remaining_activations) throw (CORBA::SystemException);

 private:

  // copy not supported
  TimeServantActivator (const TimeServantActivator & t);
  void operator = (const TimeServantActivator & t);
};

#endif // __TIME_SERVANT_ACTIVATOR_H
