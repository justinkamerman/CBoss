
#ident "$Id$"

#include "Log.h"
#include "ORBManager.h"
#include "NameServiceManager.h"


// Initialize static members
CosNaming::NamingContext_var NameServiceManager::_namingContext 
= CosNaming::NamingContext::_nil ();
 

CosNaming::NamingContext_var 
NameServiceManager::getNamingContext () throw (Exception)
{
  if (CORBA::is_nil (_namingContext))
    {
      _namingContext = CosNaming::NamingContext::_narrow 
        (ORBManager::getORB()->resolve_initial_references ("NameService"));
      debug ("NameServiceManager: resolved root naming context");
    }

  return _namingContext;
}

void NameServiceManager::bind (const char *name, 
                               const CORBA::Object_var object) 
  throw (Exception)
{
  if (CORBA::is_nil (object))
    {
      throw Exception ("NameServiceManager: bind object is null");
    }

  CosNaming::Name cosName;
  cosName.length (1);
  cosName[0].id = CORBA::string_dup (name);
  cosName[0].kind = CORBA::string_dup ("");
  
  try 
    {
      getNamingContext()->bind (cosName, object);
    }
  catch (const CORBA::Exception &exp)
    {
      throw Exception (__FILE__, __LINE__, 
                       "%s: %s", 
                       exp._name(), 
                       exp._rep_id());
    }

  debug ("NameServiceManager: bound %s", name);
}

void NameServiceManager::bind (const char *name, 
                               const char *strObject)
  throw (Exception)
{
  CORBA::Object_var object = ORBManager::getORB()->string_to_object (strObject);
  bind (name, object);
}

void NameServiceManager::rebind (const char *name, 
                                 const CORBA::Object_var object)
  throw (Exception)
{
  if (CORBA::is_nil (object))
    {
      throw Exception ("NameServiceManager: rebind object is null");
    }

  CosNaming::Name cosName;
  cosName.length (1);
  cosName[0].id = CORBA::string_dup (name);
  cosName[0].kind = CORBA::string_dup ("");

  try 
    {
      getNamingContext()->rebind (cosName, object);
    }
  catch (const CORBA::Exception &exp)
    {
      throw Exception (__FILE__, __LINE__, 
                       "%s: %s", 
                       exp._name(), 
                       exp._rep_id());
    }

  debug ("NameServiceManager: rebound %s", name);
}

void NameServiceManager::rebind (const char *name, 
                                 const char *strObject)
  throw (Exception)
{
  CORBA::Object_var object = ORBManager::getORB()->string_to_object (strObject);
  rebind (name, object);
}

void NameServiceManager::unbind (const char* name) throw (Exception)
{
  CosNaming::Name cosName;
  cosName.length (1);
  cosName[0].id = CORBA::string_dup (name);
  cosName[0].kind = CORBA::string_dup ("");

  try 
    {
      getNamingContext()->unbind (cosName);
    }
  catch (const CORBA::Exception &exp)
    {
      throw Exception (__FILE__, __LINE__, 
                       "%s: %s", 
                       exp._name(), 
                       exp._rep_id());
    }

  debug ("NameServiceManager: unbound %s", name);
}
