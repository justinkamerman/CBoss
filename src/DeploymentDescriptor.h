
#ifndef __DEPLOYMENT_DESCRIPTOR_H
#define __DEPLOYMENT_DESCRIPTOR_H

#ident "$Id$"

#include "tao/PortableServer/PortableServer.h"

#define POLICY_COUNT 7


enum PolicyListIndex 
  {
    LIFESPAN_INDEX,            
    IDASSIGNMENT_INDEX,      
    IDUNIQUENESS_INDEX,      
    IMPLICITACTIVATION_INDEX,
    REQUESTPROCESSING_INDEX,
    SERVANTRETENTION_INDEX,  
    THREAD_INDEX,
  };


class DeploymentDescriptor
{
 public:
  virtual ~DeploymentDescriptor ();

  const char *getServiceId (void) const;
  const char *getRepositoryId (void) const;
  const char *getDescription (void) const;
  const char *getServantLibrary (void) const;
  const char *getServantFactory (void) const;
  const char *getServantManagerLibrary (void) const;
  const char *getServantManagerFactory (void) const;
  const char *getDefaultServantLibrary (void) const;
  const char *getDefaultServantFactory (void) const;
  const CORBA::PolicyList *getPolicyList (void) const;

  const PortableServer::LifespanPolicyValue
    getLifeSpanPolicy (void) const;

  const PortableServer::IdAssignmentPolicyValue 
    getIdAssignmentPolicy (void) const;

  const PortableServer::IdUniquenessPolicyValue 
    getIdUniquenessPolicy (void) const;

  const PortableServer::ImplicitActivationPolicyValue 
    getImplicitActivationPolicy (void) const;

  const PortableServer::RequestProcessingPolicyValue 
    getRequestProcessingPolicy (void) const;

  const PortableServer::ServantRetentionPolicyValue 
    getServantRetentionPolicy (void) const;

  const PortableServer::ThreadPolicyValue 
    getThreadPolicy (void) const;

 protected:
  DeploymentDescriptor () {};

  DeploymentDescriptor (const char *serviceId,
                        const char *repositoryId,
                        const char *description,
                        const char *servantLibrary,
                        const char *servantFactory,
                        const char *servantManagerLibrary,
                        const char *servantManagerFactory,
                        const char *defaultServantLibrary,
                        const char *defaultServantFactory,
                        CORBA::PolicyList *policyList);

 private:
  DeploymentDescriptor (const DeploymentDescriptor &);
  DeploymentDescriptor operator = (const DeploymentDescriptor &);

  char *_serviceId; // primary key
  char *_repositoryId;
  char *_description;
  char *_servantLibrary;
  char *_servantFactory;
  char *_servantManagerLibrary;
  char *_servantManagerFactory;
  char *_defaultServantLibrary;
  char *_defaultServantFactory;
  CORBA::PolicyList *_policyList;
        
  friend class DeploymentDescriptorFactory;
};

#endif // __DEPLOYMENT_DESCRIPTOR_H
