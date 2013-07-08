
#ident "$Id$"

#include "tao/corba.h"
#include "Log.h"
#include "Exception.h"
#include "TimeServant.h"
#include "TimeZoneRegistry.h"
#include "TimeServantActivator.h"


TimeServantActivator::TimeServantActivator (void) 
{
  debug ("TimeServantActivator()");
}


TimeServantActivator::~TimeServantActivator (void)
{
  debug ("TimeServantActivator destructor:");
}


PortableServer::Servant 
TimeServantActivator::incarnate (const PortableServer::ObjectId & oid, 
                                 PortableServer::POA_ptr poa)
  throw (CORBA::SystemException , PortableServer::ForwardRequest)
{
  // Check to see if the object ID is valid (CORBA)
  CORBA::String_var oidString;
  try 
    {
      oidString = PortableServer::ObjectId_to_string (oid);
    }
  catch (const CORBA::BAD_PARAM&)
    {
      throw CORBA::OBJECT_NOT_EXIST ();
    }

  debug ("TimeServantActivator::%s(%s)", __FUNCTION__, oidString.in());

  // Check to see if object ID is valid (application)
  int offset;
  try
    {
      offset = TimeZoneRegistry::getOffset (oidString);
    }
  catch (Exception exp)
    {
      throw CORBA::OBJECT_NOT_EXIST ();
    }

  // Create servant
  PortableServer::Servant servant = new TimeServant (oidString);
  return servant;
}
 
  
void 
TimeServantActivator::etherealize (const PortableServer::ObjectId & oid, 
                                   PortableServer::POA_ptr poa,
                                   PortableServer::Servant servant,
                                   CORBA::Boolean cleanup_in_progress,
                                   CORBA::Boolean remaining_activations) 
  throw (CORBA::SystemException)
{
  // Check to see if the object ID is valid (CORBA)
  CORBA::String_var oidString;
  try 
    {
      oidString = PortableServer::ObjectId_to_string (oid);
    }
  catch (const CORBA::BAD_PARAM&)
    {
      throw CORBA::OBJECT_NOT_EXIST ();
    }

  debug ("TimeServantActivator::%s(%s)", __FUNCTION__, oidString.in());

  // Decrement servant reference count
  servant->_remove_ref ();
}

// this "C" wrapper function is reqiured to provide an 
// unmangled function symbol via which the application server
// can dynamically load and create an instance of this class
extern "C"
{
  PortableServer::ServantActivator_var servant_activator_maker (void)
  {
    return new TimeServantActivator ();
  }
}

