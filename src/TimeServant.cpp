
#ident "$Id$"

#include "orbsvcs/CosNamingC.h"
#include "Log.h"
#include "TimeServant.h"
#include "TimeZoneRegistry.h"


TimeServant::TimeServant (const char* location) 
{ 
  debug("TimeServant (%s)", location);

  _location = (char *) malloc (strlen (location) + 1);
  strcpy (_location, location);
  
  _offset = TimeZoneRegistry::getOffset(_location);
}

TimeServant::~TimeServant (void)
{
  debug ("TimeServant destructor:");
}

TimeOfDay 
TimeServant::get_time (void) throw (CORBA::SystemException)
{
  debug ("TimeServant(%s)::%s ()", _location, __FUNCTION__);

  time_t time_now = time (0);
  struct tm *time_p = gmtime (&time_now);

  TimeOfDay tod;

  if ((time_p->tm_hour + _offset) < 0)
    {
      tod.hour = ((time_p->tm_hour + _offset) % 23) + 23;
    }
  else
    {
      tod.hour = (time_p->tm_hour + _offset) % 23;
    }
    
  tod.minute = time_p->tm_min;
  tod.second = time_p->tm_sec;

  return tod;
}

// this "C" wrapper function is reqiured to provide an 
// unmangled function symbol via which the application server
// can dynamically load and create an instance of this class
extern "C"
{
  PortableServer::ServantBase_var servant_maker (const char *location)
  {
    return new TimeServant (location);
  }
}
