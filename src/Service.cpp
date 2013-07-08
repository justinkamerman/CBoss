
#ident "$Id$"

#include "Log.h"
#include "Service.h"
#include "Exception.h"


//------------------------------------------------------------
// Constructor:
//------------------------------------------------------------
Service::Service (DeploymentDescriptor *deploymentDescriptor,
                  PortableServer::POA_var poa)
  : _deploymentDescriptor (deploymentDescriptor),
    _poa (poa)
{
  debug ("Service(%s)", _deploymentDescriptor->getServiceId ());
}

//------------------------------------------------------------
// Destructor:
//------------------------------------------------------------
Service::~Service()
{
  debug ("Service destructor:");

  // Assumptions:
  // all servants use reference counting [Henning1999 p492]
  //
  // If etherealizeObjects is true and the POA has a servant manager,
  // the POA first effectively destroys itself and then invokes 
  // etherealize() on the servant manager for each servant registed 
  // in its active object map [Henning1999 p521]
  bool etherealizeObjects 
    = ((_deploymentDescriptor->getServantRetentionPolicy ()
        == PortableServer::RETAIN)
       && (_deploymentDescriptor->getRequestProcessingPolicy ()
           == PortableServer::USE_SERVANT_MANAGER)
       );

  delete (_deploymentDescriptor);

  // TODO -- how do we destroy (decrement reference count of) any 
  // servant managers ?

  _poa->destroy (etherealizeObjects, true); // blocks
}

//------------------------------------------------------------
// getState:
//------------------------------------------------------------
const PortableServer::POAManager::State Service::getState (void) const
{
  debug ("Service(%s)::%s()", 
         _deploymentDescriptor->getServiceId (),
         __FUNCTION__);

  return _poa->the_POAManager ()->get_state ();
}

//------------------------------------------------------------
// setState:
// HOLDING|ACTIVE|DISCARDING
//------------------------------------------------------------
void Service::setState (PortableServer::POAManager::State state)
{
  debug ("Service(%s)::%s()", 
         _deploymentDescriptor->getServiceId (),
         __FUNCTION__);

  switch (state) 
    {
    case PortableServer::POAManager::HOLDING:
      _poa->the_POAManager()->hold_requests (true); // blocks
      break;
      
    case PortableServer::POAManager::ACTIVE:
      _poa->the_POAManager()->activate(); // blocks
      break;
      
    case PortableServer::POAManager::DISCARDING:
      _poa->the_POAManager()->discard_requests (true); // blocks
      break;
      
    default:
      throw Exception (__FILE__, __LINE__, "Invalid state: %s", state);
    }
}


