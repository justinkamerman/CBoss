
#ifndef __ORB_MANAGER_H
#define __ORB_MANAGER_H

#ident "$Id$"

#include "ace/Log_Msg.h"
#include "tao/corba.h"
#include "tao/PortableServer/PortableServer.h"
#include "orbsvcs/CosNamingC.h"
#include "Exception.h"


class ORBManager
{
 public:
  static void init (int argc, char *argv[]);
  static CORBA::ORB_var getORB () throw (Exception);
  static PortableServer::POA_var getRootPOA () throw (Exception);
  static PortableServer::Current_var getCurrent () throw (Exception);

  virtual ~ORBManager () {};

 protected:
  ORBManager () {};
      
 private:
  ORBManager (const ORBManager &);
  ORBManager operator = (const ORBManager &);

  static CORBA::ORB_var                 _orb;
  static PortableServer::POA_var        _rootPOA;
  static CosNaming::NamingContext_var   _namingContext;
};

#endif // __ORB_MANAGER_H
