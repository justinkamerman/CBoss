
#ident "$Id$"

#include "Log.h"
#include "Exception.h"
#include "DeploymentDescriptor.h"


//------------------------------------------------------------
// Constructors:
//------------------------------------------------------------
DeploymentDescriptor::
DeploymentDescriptor (const char *serviceId,
                      const char *repositoryId,
                      const char *description,
                      const char *servantLibrary,
                      const char *servantFactory,
                      const char *servantManagerLibrary,
                      const char *servantManagerFactory,
                      const char *defaultServantLibrary,
                      const char *defaultServantFactory,
                      CORBA::PolicyList *policyList) :
  _repositoryId(0),
  _description(0), 
  _servantLibrary(0),
  _servantFactory(0),
  _servantManagerLibrary(0),
  _servantManagerFactory(0),
  _defaultServantLibrary(0),
  _defaultServantFactory(0)
  
{
  if (serviceId)
    {
      _serviceId = (char *) malloc (strlen (serviceId) + 1);
      strcpy (_serviceId, serviceId);
    }
  else
    {
      throw Exception ("Service ID cannot be null");
    }

  if (repositoryId)
    {
      _repositoryId = (char *) malloc (strlen (repositoryId) + 1);
      strcpy (_repositoryId, repositoryId);
    }
  else
    {
      throw Exception ("Repository ID cannot be null");
    }

  if (policyList)
    {
      _policyList = policyList;
    }
  else 
    {
      throw Exception ("Policy List cannot be null");
    }

  if (description)
    {
      _description = (char *) malloc (strlen (description) + 1);
      strcpy (_description, description);
    }

  if (servantLibrary)
    {
      _servantLibrary = (char *) malloc (strlen (servantLibrary) + 1);
      strcpy (_servantLibrary, servantLibrary);
    }

  if (servantFactory)
    {
      _servantFactory = (char *) malloc (strlen (servantFactory) + 1);
      strcpy (_servantFactory, servantFactory);
    }

  if (servantManagerLibrary)
    {
      _servantManagerLibrary = 
        (char *) malloc (strlen (servantManagerLibrary) + 1);
      strcpy (_servantManagerLibrary, servantManagerLibrary);
    }

  if (servantManagerFactory)
    {
      _servantManagerFactory = 
        (char *) malloc (strlen (servantManagerFactory) + 1);
      strcpy (_servantManagerFactory, servantManagerFactory);
    }

  if (defaultServantLibrary)
    {
      _defaultServantLibrary = 
        (char *) malloc (strlen (defaultServantLibrary) + 1);
      strcpy (_defaultServantLibrary, defaultServantLibrary);
    }

  if (defaultServantFactory)
    {
      _defaultServantFactory = 
        (char *) malloc (strlen (defaultServantFactory) + 1);
      strcpy (_defaultServantFactory, defaultServantFactory);
    }
}

//------------------------------------------------------------
// Destructor:
//------------------------------------------------------------
DeploymentDescriptor::~DeploymentDescriptor()
{
  debug ("DeploymentDescriptor destructor:");
  for (int i = 0; i < POLICY_COUNT; i++)
    {
      (*_policyList)[i]->destroy();
    }
    
  delete (_policyList);
  delete (_serviceId);
  delete (_description);
  delete (_servantLibrary);
  delete (_servantFactory);
  delete (_servantManagerLibrary);
  delete (_servantManagerFactory);
  delete (_defaultServantLibrary);
  delete (_defaultServantFactory);
}


const char *DeploymentDescriptor::getServiceId (void) const
{
  return _serviceId;
}

const char *DeploymentDescriptor::getRepositoryId (void) const
{
  return _repositoryId;
}

const char *DeploymentDescriptor::getDescription (void) const
{
  return _description;
}

const char *DeploymentDescriptor::getServantLibrary (void) const
{
  return _servantLibrary;
}

const char *DeploymentDescriptor::getServantFactory (void) const
{
  return _servantFactory;
}

const char *DeploymentDescriptor::getServantManagerLibrary (void) const
{
  return _servantManagerLibrary;
}

const char *DeploymentDescriptor::getServantManagerFactory (void) const
{
  return _servantManagerFactory;
}

const char *DeploymentDescriptor::getDefaultServantLibrary (void) const
{
  return _defaultServantLibrary;
}

const char *DeploymentDescriptor::getDefaultServantFactory (void) const
{
  return _defaultServantFactory;
}

const CORBA::PolicyList *DeploymentDescriptor::getPolicyList (void) const
{
  return _policyList;
}

const PortableServer::LifespanPolicyValue
DeploymentDescriptor::getLifeSpanPolicy (void) const
{
  return (PortableServer::LifespanPolicy::
          _narrow ((*_policyList)[LIFESPAN_INDEX]))->value();
}

const PortableServer::IdAssignmentPolicyValue 
DeploymentDescriptor::getIdAssignmentPolicy (void) const 
{
  return (PortableServer::IdAssignmentPolicy::
          _narrow ((*_policyList)[IDASSIGNMENT_INDEX]))->value();
}
  
const PortableServer::IdUniquenessPolicyValue 
DeploymentDescriptor::getIdUniquenessPolicy (void) const
{
  return (PortableServer::IdUniquenessPolicy::
          _narrow ((*_policyList)[IDUNIQUENESS_INDEX]))->value();
}
 
const PortableServer::ImplicitActivationPolicyValue 
DeploymentDescriptor::getImplicitActivationPolicy (void) const 
{
  return (PortableServer::ImplicitActivationPolicy::
          _narrow ((*_policyList)[IMPLICITACTIVATION_INDEX]))->value(); 
}
  
const PortableServer::RequestProcessingPolicyValue 
DeploymentDescriptor::getRequestProcessingPolicy (void) const 
{
  return (PortableServer::RequestProcessingPolicy::
          _narrow ((*_policyList)[REQUESTPROCESSING_INDEX]))->value();
}
  
const PortableServer::ServantRetentionPolicyValue 
DeploymentDescriptor::getServantRetentionPolicy (void) const 
{
  return (PortableServer::ServantRetentionPolicy::
          _narrow ((*_policyList)[SERVANTRETENTION_INDEX]))->value();       
}
  
const PortableServer::ThreadPolicyValue 
DeploymentDescriptor::getThreadPolicy (void) const 
{
  return (PortableServer::ThreadPolicy::
          _narrow ((*_policyList)[THREAD_INDEX]))->value();
}


