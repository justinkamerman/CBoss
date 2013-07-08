
#ident "$Id$"

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <dlfcn.h>
#include "orbsvcs/CosNamingC.h"

#include "DeploymentDescriptorFactory.h"
#include "Exception.h"
#include "Log.h"
#include "ORBManager.h"
#include "POAFactory.h"
#include "ServiceManager.h"


typedef PortableServer::ServantBase_var (*SERVANT_FACTORY) (const char *);
typedef PortableServer::ServantBase_var (*DEFAULT_SERVANT_FACTORY)
  (PortableServer::Current_ptr current);
typedef PortableServer::ServantActivator_var (*SERVANT_ACTIVATOR_FACTORY) 
  (void);
typedef PortableServer::ServantLocator_var (*SERVANT_LOCATOR_FACTORY) 
  (void);

// Initialize singleton instance
ServiceManager* ServiceManager::_instance = 0;

  
//------------------------------------------------------------
// Destructor:
// - destroy active service map
//------------------------------------------------------------
ServiceManager::~ServiceManager ()
{
  ServiceMap::iterator serviceMapIterator;
  for (serviceMapIterator = _activeServiceMap.begin();
       serviceMapIterator != _activeServiceMap.end();
       serviceMapIterator++)
    {
      debug ("ServantManager destructor: destroying service '%s'",
             serviceMapIterator->second->getServiceId ());
      delete (serviceMapIterator->second);
    }
}

//------------------------------------------------------------
// instance: 
// lazy initialize and return singleton instance.
//------------------------------------------------------------
ServiceManager* ServiceManager::instance ()
{
  if ( _instance == 0 ) 
    _instance = new ServiceManager();
  
  return _instance;
}

//------------------------------------------------------------
// startService:
//
//------------------------------------------------------------
void ServiceManager::createService (const char *serviceId)
{
  DeploymentDescriptor *deploymentDescriptor;
  PortableServer::POA_var poa;

  // Check that service is not already active. We are forced to 
  // make a local non-const copy of serviceId because of find().
  char *localServiceId = (char *) malloc (strlen (serviceId) + 1);
  strcpy (localServiceId, serviceId);
  ServiceMap::iterator serviceMapIterator;
  serviceMapIterator = _activeServiceMap.find (localServiceId);
    
  if (serviceMapIterator != _activeServiceMap.end())
    {
      throw Exception (__FILE__, __LINE__, 
                       "Service already exists: %s", 
                       serviceId);
    }
    
  // Get the service's deployment descriptor
  deploymentDescriptor 
    = DeploymentDescriptorFactory::instance()->create(serviceId);

  try 
    { 
      // Create POA 
      poa = POAFactory::create (serviceId, 
                                deploymentDescriptor->getPolicyList());
      // Servants:
      // create and register either:
      // 1. servant           (USE_ACTIVE_OBJECT_MAP_ONLY)
      // 2. servant activator (USE_SERVANT_MANAGER + RETAIN)
      // 3. servant locator   (USE_SERVANT_MANAGER + NON_RETAIN)
      // 4. default servant   (USE_DEFAULT_SERVANT)
        
      if (deploymentDescriptor->getRequestProcessingPolicy () 
          == PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY)
        {
          // servants are created when objects are activated
        }
      else if ((deploymentDescriptor->getRequestProcessingPolicy () 
                == PortableServer::USE_SERVANT_MANAGER)
               && (deploymentDescriptor->getServantRetentionPolicy () 
                   == PortableServer::RETAIN))
        {
          // Compare [Henning1999 p480] (CORBA 2.x) to below (CORBA 3.x)
          //
          // Hi Jules:
          //
          // >> I have e.g. been looking in the Henning & Vinoski book in the
          // >> section "Servant Manager Registration" on page 480. In this 
          // >> section the activator object is implicitly registered under
          // >> the root POA by a _this() operation. Is this registration 
          // >> done in the background in TAO or am I missing something ??
          //
          // H&V use the _this() operator to go from the ServantActivator 
          // servant to the ServantActivator object.  After this, they call
          // set_servant_manager().  With the ServantActivator becoming a 
          // "local" interface (see section 4.1.5 of CORBA Components 
          // Volume I - orbos/99-07-01), the ServantActivator implementation 
          // directly inherits from the stub and not from the skeleton.  
          // Therefore, you do not need to perform _this() to go from servant 
          // to object.  However, you still need to call 
          // set_servant_manager().
          //
          // Irfan
         
          // Dynamically load servant activator factory 
          SERVANT_ACTIVATOR_FACTORY servantActivatorFactory =
            (SERVANT_ACTIVATOR_FACTORY) loadSymbol
            (deploymentDescriptor->getServantManagerLibrary(),
             deploymentDescriptor->getServantManagerFactory());

          // Create a servant activator object
          PortableServer::ServantActivator_var servantActivator =
            servantActivatorFactory();

          // Register the servant activator with the POA
          poa->set_servant_manager (servantActivator);

        }
      else if ((deploymentDescriptor->getRequestProcessingPolicy () 
                == PortableServer::USE_SERVANT_MANAGER)
               && (deploymentDescriptor->getServantRetentionPolicy ()
                   == PortableServer::NON_RETAIN))
        {
          // Compare [Henning1999 p480] (CORBA 2.x) to below (CORBA 3.x)

          // Dynamically load servant locator factory 
          SERVANT_LOCATOR_FACTORY servantLocatorFactory =
            (SERVANT_LOCATOR_FACTORY) loadSymbol
            (deploymentDescriptor->getServantManagerLibrary(),
             deploymentDescriptor->getServantManagerFactory());
          
          // Create a servant locator object
          PortableServer::ServantLocator_var servantLocator =
            servantLocatorFactory();
          
          // Register the servant locator with the POA
          poa->set_servant_manager (servantLocator);
          
        }
      else if ((deploymentDescriptor->getRequestProcessingPolicy () 
                == PortableServer::USE_DEFAULT_SERVANT))
        {
          // Dynamically load default servant constructor, 
          // instantiate and register with POA [Henning1999 p484]
          DEFAULT_SERVANT_FACTORY defaultServantFactory = 
            (DEFAULT_SERVANT_FACTORY) loadSymbol 
            (deploymentDescriptor->getDefaultServantLibrary(),
             deploymentDescriptor->getDefaultServantFactory());
          
          PortableServer::ServantBase_var defaultServant = 
            defaultServantFactory (ORBManager::getCurrent());

          poa->set_servant (defaultServant.in());

          debug ("ServiceManager: registered default servant");
        }

      // Add service object to active service map
      _activeServiceMap [localServiceId] = new Service (deploymentDescriptor,
                                                        poa);
    }
  catch (const CORBA::Exception &exp) 
    {
      throw Exception (__FILE__, __LINE__, 
                       "%s: %s", 
                       exp._name(), 
                       exp._rep_id());
    }
    
  debug ("ServiceManager: created service %s", serviceId);
}
  
//------------------------------------------------------------
// destroyService:
//------------------------------------------------------------
void ServiceManager::destroyService (const char *serviceId)
{
  Service *service = getService (serviceId);

  char *localServiceId = (char *) malloc (strlen (serviceId) + 1);
  strcpy (localServiceId, serviceId);
  _activeServiceMap.erase (localServiceId);

  delete (service);
}

//------------------------------------------------------------
// getServiceStatus:
//------------------------------------------------------------
PortableServer::POAManager::State 
ServiceManager::getServiceState (const char *serviceId)
{
  return getService (serviceId)->getState ();
}

//------------------------------------------------------------
// setServiceStatus:
//------------------------------------------------------------
void ServiceManager::setServiceState 
(const char *serviceId, PortableServer::POAManager::State state)
{
  getService (serviceId)->setState (state);
}

//------------------------------------------------------------
// getService:
//------------------------------------------------------------
Service *ServiceManager::getService (const char *serviceId)
{
  char *localServiceId = (char *) malloc (strlen (serviceId) + 1);
  strcpy (localServiceId, serviceId);
  ServiceMap::iterator serviceMapIterator = 
    _activeServiceMap.find (localServiceId);
    
  if (serviceMapIterator == _activeServiceMap.end())
    {
      throw Exception (__FILE__, __LINE__, 
                       "Service does not exist: %s", 
                       serviceId);
    }
  else
    {
      return serviceMapIterator->second;
    }
}

//------------------------------------------------------------
// activateObject:
//
// in serviceId:      identifies the service within which to activate
//                    the object
// in objectId:       id to assign object for USER_ID policy. For
//                    SYSTEM_ID policy, an id is assigned by the 
//                    system and this parameter is ignored
// out:               the objectId of the activated object
//------------------------------------------------------------
char *ServiceManager::activateObject (const char *serviceId,
                                      const char *objectId)
{
  PortableServer::ObjectId_var oid;
  CORBA::Object_var object;
  const Service *service = getService (serviceId);

  const DeploymentDescriptor *deploymentDescriptor = 
    service->getDeploymentDescriptor ();

  const PortableServer::POA_ptr poa = service->getPOA ();

  if ((deploymentDescriptor->getIdAssignmentPolicy () 
       == PortableServer::USER_ID) 
      && (objectId == 0))
    {
      throw Exception (__FILE__, __LINE__,
                       "Service %s: an object id must be given for "
                       "USER_ID policy",
                       service->getServiceId ());
    }
    
  if ((deploymentDescriptor->getIdAssignmentPolicy () 
       == PortableServer::SYSTEM_ID) 
      && (objectId != 0))
    {
      debug ("Service %s: ignoring object id for activation due "
             "to SYSTEM_ID policy",
             service->getServiceId ());
    }

  // USE_ACTIVE_OBJECT_MAP_ONLY
  if (deploymentDescriptor->getRequestProcessingPolicy () 
      == PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY)
    {
      SERVANT_FACTORY servantFactory = (SERVANT_FACTORY) loadSymbol 
        (deploymentDescriptor->getServantLibrary(),
         deploymentDescriptor->getServantFactory());

      PortableServer::ServantBase_var servant = servantFactory (objectId);

      if (deploymentDescriptor->getIdAssignmentPolicy () 
          == PortableServer::SYSTEM_ID)
        {       
          // SYSTEM_ID
          oid = poa->activate_object (servant.inout());
        }
      else
        {
          // USER_ID
          oid = PortableServer::string_to_ObjectId (objectId);
          poa->activate_object_with_id (oid.in(), servant.inout());
        }

      // Neither of the activation operations returns the object
      // reference of the new object. One way to do this is to 
      // use POA::id_to_reference().
      object = poa->id_to_reference (oid);
    }

  // USE_SERVANT_MANAGER | USE_DEFAULT_SERVANT
  else
    {
      if (deploymentDescriptor->getIdAssignmentPolicy () 
          == PortableServer::SYSTEM_ID)
        {
          // TODO: describe repository Id in doc {Henning1999 p 116]

          // SYSTEM_ID
          object = poa->create_reference 
            (deploymentDescriptor->getRepositoryId ());
        }
      else 
        {
          // USER_ID
          oid = PortableServer::string_to_ObjectId (objectId);
          object = poa->create_reference_with_id 
            (oid, deploymentDescriptor->getRepositoryId ());
        }
    }

  CORBA::String_var strObjectReference = 
    ORBManager::getORB()->object_to_string (object);
  
  // Log object id
  CORBA::String_var string = PortableServer::ObjectId_to_string (oid.in ());
  
  debug ("ServiceManager: activated object '%s' under service '%s'", 
         string.in (),
         service->getServiceId());
  
  return strObjectReference._retn();
}

//------------------------------------------------------------
// deactivateObject:
//------------------------------------------------------------
void ServiceManager::deactivateObject (const char *serviceId,
                                       const char *objectId)
{
  const Service *service = getService (serviceId);
  PortableServer::ObjectId_var oid;

  oid = PortableServer::string_to_ObjectId (objectId);   
  service->getPOA ()->deactivate_object (oid);

  debug ("Service: %s: deactivated object %s", serviceId, objectId);
}


//------------------------------------------------------------
// loadSymbol:
//------------------------------------------------------------
void *ServiceManager::loadSymbol (const char *libraryName,
                                  const char *symbolName)
{
  void *handle = dlopen (libraryName, RTLD_NOW);

  if (!handle)
    {
      throw Exception (__FILE__, __LINE__, dlerror());
    }
  
  void *symbol =  dlsym (handle, symbolName);
  char *dlError = dlerror();
  if (dlError)
    {
      throw Exception (__FILE__, __LINE__,
                       "Error loading symbol %s from library %s: %s", 
                       symbolName,
                       libraryName,
                       dlError);
    }

  // closing the library prematurely causes a segmentation fault
  // when we reference any symbols loaded from the library.
  //dlclose (handle);

  return symbol;
}


