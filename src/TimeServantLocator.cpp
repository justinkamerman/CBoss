
#ident "$Id$"

#include "tao/corba.h"
#include "Log.h"
#include "Exception.h"
#include "TimeServant.h"
#include "TimeZoneRegistry.h"
#include "TimeServantLocator.h"


TimeServantLocator::TimeServantLocator (void) 
{
  debug ("TimeServantLocator()");
}


TimeServantLocator::~TimeServantLocator (void)
{
  debug ("TimeServantLocator() destructor:");
}


PortableServer::Servant 
TimeServantLocator::preinvoke (const PortableServer::ObjectId & oid, 
                               PortableServer::POA_ptr poa,
                               const char *operation,
                               void * &cookie)
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

  debug ("TimeServantLocator::%s(%s)", __FUNCTION__, oidString.in());

  // Is there an existing servant for this object
  // TODO

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
TimeServantLocator::postinvoke (const PortableServer::ObjectId & oid, 
                                PortableServer::POA_ptr poa,
                                const char *operation,
                                void *cookie,
                                PortableServer::Servant servant)
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

  debug ("TimeServantLocator::%s(%s)", __FUNCTION__, oidString.in());

  // Decrement servant reference count
  servant->_remove_ref ();
}

// this "C" wrapper function is reqiured to provide an 
// unmangled function symbol via which the application server
// can dynamically load and create an instance of this class
extern "C"
{
  PortableServer::ServantLocator_var servant_locator_maker (void)
  {
    return new TimeServantLocator ();
  }
}

