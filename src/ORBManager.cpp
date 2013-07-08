
#ident "$Id$"

#include "Log.h"
#include "ORBManager.h"


// Initialize static members
CORBA::ORB_var ORBManager::_orb = CORBA::ORB::_nil ();
PortableServer::POA_var  ORBManager::_rootPOA = PortableServer::POA::_nil ();
CosNaming::NamingContext_var ORBManager::_namingContext 
= CosNaming::NamingContext::_nil ();

void 
ORBManager::init (int argc, char *argv[])
{
  _orb = CORBA::ORB_init (argc, argv);
  debug ("ORBManager initialized ORB");
}

CORBA::ORB_var 
ORBManager::getORB () throw (Exception)
{
  if (CORBA::is_nil (_orb)) 
    {
      throw Exception (__FILE__, __LINE__,
                       "ORB has not been initialised");
    }
    
  return _orb;
}

PortableServer::POA_var 
ORBManager::getRootPOA () throw (Exception)
{
  if (CORBA::is_nil (_orb)) 
    throw Exception (__FILE__, __LINE__,
                     "ORB has not been initialised");
    
  if (CORBA::is_nil (_rootPOA))
    {
      CORBA::Object_var obj = 
        _orb->resolve_initial_references ("RootPOA");
      _rootPOA = PortableServer::POA::_narrow (obj.in ());
      debug ("ORBManager resolved root POA reference");
    }
    
  return _rootPOA;
}     

 PortableServer::Current_var 
ORBManager::getCurrent () throw (Exception)
{
  if (CORBA::is_nil (_orb)) 
    throw Exception (__FILE__, __LINE__,
                     "ORB has not been initialised");

  CORBA::Object_var obj = _orb->resolve_initial_references ("POACurrent");
  PortableServer::Current_var current = PortableServer::Current::_narrow (obj);

  return current;
}
