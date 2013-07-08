
#ifndef __SERVICE_MANAGER_H
#define __SERVICE_MANAGER_H

#ident "$Id$"

#include <string>
#include <map>
#include "ace/Log_Msg.h"
#include "Service.h"


// [Stroustrup1997 17.1.4.1] 
struct Cstring_less
{
  bool operator () (const char *p, const char *q) const 
  {
    return strcmp (p, q) < 0;
  }
};


typedef std::map <char *, Service *, Cstring_less> ServiceMap;


class ServiceManager
{
 public:
  static ServiceManager *instance (void);

  // service methods
  virtual void createService (const char *serviceId);
  virtual void destroyService (const char *serviceId);  
  virtual PortableServer::POAManager::State 
    getServiceState (const char *serviceId);
  virtual void setServiceState (const char *serviceId, 
                                PortableServer::POAManager::State state);

  // object methods
  virtual char *activateObject (const char *serviceId,
                                const char *objectId);
  virtual void deactivateObject (const char *serviceId,
                                 const char *objectId);
                                    

  virtual ~ServiceManager ();

 protected:
  ServiceManager () {};
      
 private:
  Service *getService (const char *serviceId);
  void *loadSymbol (const char *libraryName, const char *symbolName);
  ServiceManager (const ServiceManager &);
  ServiceManager operator = (const ServiceManager &);

  static ServiceManager *_instance;
  ServiceMap _activeServiceMap;

};

#endif // __SERVICE_MANAGER_H
