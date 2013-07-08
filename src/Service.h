
#ifndef __SERVICE_H
#define __SERVICE_H

#ident "$Id$"

#include "tao/PortableServer/PortableServer.h"
#include "tao/PortableServer/Servant_Base.h"
#include "DeploymentDescriptor.h"


class Service
{
 public:
  virtual ~Service ();
      
  const PortableServer::POAManager::State Service::getState (void) const;
  void Service::setState (PortableServer::POAManager::State state);

  const char *Service::getServiceId (void) const
    {
      return _deploymentDescriptor->getServiceId ();
    }
      
  const char *Service::getDescription (void) const
    {
      return _deploymentDescriptor->getDescription ();
    }
      
  const PortableServer::POA_ptr getPOA (void) const
    {
      return _poa;
    }

  const DeploymentDescriptor *getDeploymentDescriptor (void) const
    {
      return _deploymentDescriptor;
    }
      
 protected:
  Service () {};

  Service (DeploymentDescriptor *deploymentDescriptor,
           PortableServer::POA_var poa);
  Service (const Service &);
  Service operator = (const Service &);
      
  DeploymentDescriptor *_deploymentDescriptor;
  PortableServer::POA_var _poa;
  
  friend class ServiceManager;
};

#endif // __SERVICE_H
