
#ifndef __NAME_SERVICE_MANAGER_H
#define __NAME_SERVICE_MANAGER_H

#ident "$Id$"

#include "ace/Log_Msg.h"
#include "tao/corba.h"
#include "tao/PortableServer/PortableServer.h"
#include "orbsvcs/CosNamingC.h"
#include "Exception.h"


class NameServiceManager
{
 public:
  static CosNaming::NamingContext_var getNamingContext () 
    throw (Exception);
  static void bind (const char *name, const CORBA::Object_var object) 
    throw (Exception);
  static void bind (const char *name, const char *strObject) 
    throw (Exception);
  static void rebind (const char *name, const CORBA::Object_var object)
    throw (Exception);
  static void rebind (const char *name, const char *strObject) 
    throw (Exception);
  static void unbind (const char* name) throw (Exception);

  virtual ~NameServiceManager () {};

 protected:
  NameServiceManager () {};
      
 private:
  NameServiceManager (const NameServiceManager &);
  NameServiceManager operator = (const NameServiceManager &);

  static CosNaming::NamingContext_var   _namingContext;
};

#endif // __NAME_SERVICE_MANAGER_H
