
#ident "$Id$"

#include "Log.h"
#include "ORBManager.h"
#include "POAFactory.h"


// create: 
// Create a new POA with the given policies and servant manager 
// installed. The product is a child of the root POA.
PortableServer::POA_var
POAFactory::create (const char *serviceId,
                    const CORBA::PolicyList *policyList)
{  
  // Passing a nil POAManager argument to create_POA() causes 
  // a new POAManager to be created for the product POA. This allows
  // us to control the flow of requests into each POA independently.
  // [Henning1999 p448]
  // 
  // However, it seems that if we follow this approach, the service 
  // doesnt work with the Implementation Repository. The POA doesnt 
  // seem to respond to pinging by the Implementation Activator which 
  // is how the Activator is supposed to know when the service comes up.
  // 
  // As a workaround, we are using the root POAManager for all POAs.
  // As a result, changing the state of any of the POAs, changes the
  // state of all of them. 
  PortableServer::POAManager_var rootPOAManager = 
    ORBManager::getRootPOA ()->the_POAManager ();

  PortableServer::POA_var newPOA = ORBManager::getRootPOA ()->
    create_POA (serviceId, rootPOAManager, *policyList);

  debug ("POAFactory: created POA: %s", serviceId);
  return newPOA;
}     

