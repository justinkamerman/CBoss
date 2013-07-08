
#ifndef __DEPLOYMENT_DESCRIPTOR_FACTORY_H
#define __DEPLOYMENT_DESCRIPTOR_FACTORY_H

#ident "$Id$"

#include <string>
#include <xercesc/dom/DOM.hpp>
#include "DeploymentDescriptor.h"


class DeploymentDescriptorFactory
{
 public:
  static DeploymentDescriptorFactory* instance (void);
  virtual DeploymentDescriptor* create (const char *serviceId);
  virtual ~DeploymentDescriptorFactory ();

 protected:
  DeploymentDescriptorFactory ();

 private:
  DeploymentDescriptorFactory (const DeploymentDescriptorFactory &);
  DeploymentDescriptorFactory operator = 
    (const DeploymentDescriptorFactory &);

  static DeploymentDescriptorFactory* _instance;
  bool XMLPlatformUtilsInitialized; 
  DOMBuilder *parser;
  DOMDocument *doc;
};

#endif // __DEPLOYMENT_DESCRIPTOR_FACTORY_H
