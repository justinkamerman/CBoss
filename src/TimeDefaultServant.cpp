
#ident "$Id$"

#include "Log.h"
#include "Exception.h"
#include "TimeZoneRegistry.h"
#include "TimeDefaultServant.h"


TimeDefaultServant::TimeDefaultServant (PortableServer::Current_ptr current) 
  : _current (PortableServer::Current::_duplicate(current))
{
  debug ("%s()", __FUNCTION__);
}

TimeDefaultServant::~TimeDefaultServant (void)
{
  debug ("TimeDefaultServant destructor:");
}

TimeOfDay 
TimeDefaultServant::get_time (void) throw (CORBA::SystemException)
{
  // For each request, determine the object id (location) upon 
  // which the request was made and retrieve the time zone offset.
  int offset;
  char *oidString = getObjectId ();
  debug ("TimeDefaultServant(%s)::%s ()", oidString, __FUNCTION__);

  try
    {
      offset = TimeZoneRegistry::getOffset (oidString);
    }
  catch (Exception exp)
    {
      throw CORBA::OBJECT_NOT_EXIST();
    }

  time_t time_now = time (0);
  struct tm *time_p = gmtime (&time_now);

  TimeOfDay tod;

  if ((time_p->tm_hour + offset) < 0)
    {
      tod.hour = ((time_p->tm_hour + offset) % 23) + 23;
    }
  else
    {
      tod.hour = (time_p->tm_hour + offset) % 23;
    }

  tod.minute = time_p->tm_min;
  tod.second = time_p->tm_sec;

  delete (oidString);
  return tod;
}

// Within the context of a request dispatch, the server ORB allows 
// an application to obtain the ObjectId of the target object and
// a reference to the POA that is dispatchig the request. These 
// operations are provided by the PortableServer::Current interface.
// Ref [Henning1999 p488]
char *TimeDefaultServant::getObjectId ()
{
  // get the target objectId
  PortableServer::ObjectId_var oid = _current->get_object_id ();

  // check to see if the objectId is valid
  CORBA::String_var oidString;
  try
    {
      oidString = PortableServer::ObjectId_to_string (oid);
    }
  catch (const CORBA::BAD_PARAM&)
    {
      throw CORBA::OBJECT_NOT_EXIST ();
    }

  return oidString._retn ();
}


// this "C" wrapper function is reqiured to provide an 
// unmangled function symbol via which the application server
// can dynamically load and create an instance of this class
extern "C"
{
  PortableServer::ServantBase_var default_servant_maker 
  (PortableServer::Current_ptr current)
  {
    return new TimeDefaultServant (current);
  }
}
