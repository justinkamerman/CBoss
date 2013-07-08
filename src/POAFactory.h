
#ifndef __POA_FACTORY_H
#define __POA_FACTORY_H

#ident "$Id$"

#include "ace/Log_Msg.h"
#include "tao/corba.h"
#include "tao/PortableServer/PortableServer.h"
#include "Exception.h"


class POAFactory
{
 public:
  static PortableServer::POA_var create (const char *serviceId,
                                         const CORBA::PolicyList *policyList);

  virtual ~POAFactory () {};

 protected:
  POAFactory () {};
      
 private:
  POAFactory (const POAFactory &);
  POAFactory operator = (const POAFactory &);

};

#endif // __POA_FACTORY_H
