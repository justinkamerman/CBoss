
#ident "$Id$"

// ---------------------------------------------------------------------------
//  Includes
// ---------------------------------------------------------------------------
#include <iostream>
#include <fstream>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/parsers/AbstractDOMParser.hpp>
#include "tao/PortableServer/PortableServer.h"
#include "Log.h"
#include "Exception.h"
#include "DeploymentDescriptorFactory.h"
#include "ORBManager.h"


// ---------------------------------------------------------------------------
// DOMCountErrorHandler
// simple error handler deriviative to install on parser
// ---------------------------------------------------------------------------
class DOMCountErrorHandler : public DOMErrorHandler
{
public:
  // -----------------------------------------------------------------------
  //  Constructors and Destructor
  // -----------------------------------------------------------------------
  DOMCountErrorHandler();
  ~DOMCountErrorHandler();
  
  
  // -----------------------------------------------------------------------
  //  Getter methods
  // -----------------------------------------------------------------------
  bool getSawErrors() const;
  
  
  // -----------------------------------------------------------------------
  //  Implementation of the DOM ErrorHandler interface
  // -----------------------------------------------------------------------
  bool handleError(const DOMError& domError);
  void resetErrors();
  
  
private :
  // -----------------------------------------------------------------------
  //  Unimplemented constructors and operators
  // -----------------------------------------------------------------------
  DOMCountErrorHandler(const DOMCountErrorHandler&);
  void operator=(const DOMCountErrorHandler&);
  
  
  // -----------------------------------------------------------------------
  //  Private data members
  // -----------------------------------------------------------------------
  
  // fSawErrors
  // This is set if we get any errors, and is queryable via a getter
  // method. Its used by the main code to suppress output if there are
  // errors.
  bool    fSawErrors;
};


// ---------------------------------------------------------
// This is a simple class that lets us do easy (though not 
// terribly efficient) trancoding of XMLCh data to local 
// code page for display.
// ---------------------------------------------------------
class StrX
{
public :
  // -----------------------------------------------------
  //  Constructors and Destructor
  // -----------------------------------------------------
  StrX(const XMLCh* const toTranscode)
  {
    // Call the private transcoding method
    fLocalForm = XMLString::transcode(toTranscode);
  }
  
  ~StrX()
  {
    delete [] fLocalForm;
  }
  
  // -----------------------------------------------------
  //  Getter methods
  // -----------------------------------------------------
  const char* localForm() const
  {
    return fLocalForm;
  }
  
private :
  //------------------------------------------------------
  //  Private data members
  //
  //  fLocalForm
  //      This is the local code page form of the string.
  // -----------------------------------------------------
  char*   fLocalForm;
};

inline ostream& operator<<(ostream& target, const StrX& toDump)
{
  target << toDump.localForm();
  return target;
}

inline bool DOMCountErrorHandler::getSawErrors() const
{
  return fSawErrors;
}


//------------------------------------------------------------
// Parser error handler
//------------------------------------------------------------
DOMCountErrorHandler::DOMCountErrorHandler() : fSawErrors(false)
{
}

DOMCountErrorHandler::~DOMCountErrorHandler()
{
}


// ----------------------------------------------------------
//  DOMCountHandlers: Overrides of the DOM ErrorHandler interface
// ----------------------------------------------------------
bool DOMCountErrorHandler::handleError(const DOMError& domError)
{
  fSawErrors = true;
  if (domError.getSeverity() == DOMError::DOM_SEVERITY_WARNING)
    error ("Warning at file: ");
  else if (domError.getSeverity() == DOMError::DOM_SEVERITY_ERROR)
    error ("Error at file: ");
  else
    error ("Fatal Error at file: ");
  
  error ("%s, line %d, char %d, Message: %s",
         StrX(domError.getLocation()->getURI()).localForm(),
         domError.getLocation()->getLineNumber(),
         domError.getLocation()->getColumnNumber(),
         StrX(domError.getMessage()).localForm());
  
  return false;
}

void DOMCountErrorHandler::resetErrors()
{
  fSawErrors = false;
}


// Initialize singleton instance
DeploymentDescriptorFactory* DeploymentDescriptorFactory::_instance = 0;

  
//------------------------------------------------------------
// Default constructor: 
// initializes theXML parse and parses the deployment descriptor 
// definition file. This will probabaly change to take a file name
// or input stream argument.
//------------------------------------------------------------
DeploymentDescriptorFactory::DeploymentDescriptorFactory()
{
  const char *xmlFile = "DeploymentDescriptor.xml";
    
  // Initialize the XML4C system
  try
    {
      XMLPlatformUtils::Initialize();
      XMLPlatformUtilsInitialized = true;
    }
    
  catch (const XMLException& exp) {
    throw Exception ("Error initializing XMLPlatformUtils: %s",
                     XMLString::transcode (exp.getMessage()));
  }
    
  // Create the DOM parser.
  static const XMLCh gLS[] = { chLatin_L, chLatin_S, chNull };
  DOMImplementation *impl = 
    DOMImplementationRegistry::getDOMImplementation(gLS);
  parser = ((DOMImplementationLS*)impl)->createDOMBuilder
    (DOMImplementationLS::MODE_SYNCHRONOUS, 0);
    
  // Set parser features
  parser->setFeature (XMLUni::fgDOMValidation, true);
    
  // Create and install custom error handler
  DOMCountErrorHandler errorHandler;
  parser->setErrorHandler(&errorHandler);
    
  // Reset error count
  errorHandler.resetErrors();
    
  try
    {
      doc = parser->parseURI(xmlFile);
    }
    
  catch (const XMLException& exp)
    {
      error ("Error parsing file %s: %s",
             xmlFile,
             StrX(exp.getMessage()).localForm());
    }
  catch (const DOMException& exp) 
    {
      error ("DOM error parsing file %s: %d", xmlFile, exp.code);
    }
  catch (...)
    {
      error ("Unexpected exception while parsing file %s", xmlFile);
    }
    
    
  if (errorHandler.getSawErrors() || !doc)
    {
      throw Exception (__FILE__, 
                       __LINE__,
                       "Failed creating DeploymentDescriptorFactory");
    }
  else
    {
      debug ("DeploymentDescriptorFactory initialized. "
             "Configuration read from file %s", 
             xmlFile);
    }
}

//------------------------------------------------------------
// Destructor:
// delete the parser and terminate (must be in this order)
//------------------------------------------------------------
DeploymentDescriptorFactory::~DeploymentDescriptorFactory()
{
  if (XMLPlatformUtilsInitialized) 
    {
      parser->release();
      XMLPlatformUtils::Terminate();
    }
}


//------------------------------------------------------------
// instance: 
// lazy initialize and return singleton instance.
//------------------------------------------------------------
DeploymentDescriptorFactory* DeploymentDescriptorFactory::instance ()
{
  if ( _instance == 0 ) 
    _instance = new DeploymentDescriptorFactory();
  
  return _instance;
}


//------------------------------------------------------------
// create:
// create deployment descriptor for a particular service
//------------------------------------------------------------
DeploymentDescriptor* 
DeploymentDescriptorFactory::create (const char *serviceId)
{
  char *repositoryId            = 0;
  char *description             = 0;
  char *servantLibrary          = 0;
  char *servantFactory          = 0;
  char *servantManagerLibrary   = 0;
  char *servantManagerFactory   = 0;
  char *defaultServantLibrary   = 0;
  char *defaultServantFactory   = 0;
  PortableServer::LifespanPolicy_ptr lifespan;
  PortableServer::IdAssignmentPolicy_ptr idAssignment;
  PortableServer::IdUniquenessPolicy_ptr idUniqueness;
  PortableServer::ImplicitActivationPolicy_ptr implicitActivation;
  PortableServer::RequestProcessingPolicy_ptr requestProcessing;
  PortableServer::ServantRetentionPolicy_ptr servantRetention;
  PortableServer::ThreadPolicy_ptr thread;

  // Using the serviceId as a key, find the DOM element
  // for the service in the deployment descriptor document.
  DOMElement *deService = 
    doc->getElementById (XMLString::transcode (serviceId));
    
  if (deService == NULL) 
    {
      throw Exception (__FILE__,
                       __LINE__,
                       "Service definition not found: %s",
                       serviceId);
    }
    
  DOMNodeList *dnl;
  PortableServer::POA_var rootPOA = ORBManager::getRootPOA ();

  //------------------------------
  // RepositoryId
  //------------------------------
  repositoryId = XMLString::transcode 
    (deService->getAttribute( XMLString::transcode ("repositoryId")));
    
  //------------------------------
  // Description
  //------------------------------
  dnl = deService->getElementsByTagName (XMLString::transcode ("Description"));
  if (dnl->getLength() > 0) // optional element
    {
      DOMNode *dnDescription = dnl->item (0);
      DOMNode *dnDescriptionText = dnDescription->getFirstChild ();         
      description = XMLString::transcode (dnDescriptionText->getNodeValue());
    }
    
  //------------------------------
  // Servant
  //------------------------------   
  dnl = deService->getElementsByTagName ( XMLString::transcode ("Servant"));
  if (dnl->getLength() > 0) // optional element
    {
      DOMElement *dnServant = (DOMElement *) dnl->item (0);
      servantLibrary = XMLString::transcode 
        (dnServant->getAttribute( XMLString::transcode ("library")));
      servantFactory = XMLString::transcode 
        (dnServant->getAttribute( XMLString::transcode ("factory")));
    }

  //------------------------------
  // Servant Manager
  //------------------------------
  dnl = deService->getElementsByTagName
    (XMLString::transcode ("ServantManager"));
  if (dnl->getLength() > 0) // optional element
    {
      DOMElement *dnServantManager = (DOMElement *) dnl->item (0);
      servantManagerLibrary = XMLString::transcode 
        (dnServantManager->getAttribute( XMLString::transcode ("library")));
      servantManagerFactory = XMLString::transcode 
        (dnServantManager->getAttribute( XMLString::transcode ("factory")));
    }
      
  //------------------------------
  // Default Servant 
  //------------------------------
  dnl = deService->getElementsByTagName 
    ( XMLString::transcode ("DefaultServant"));
  if (dnl->getLength() > 0) // optional element
    {
      DOMElement *dnDefaultServant = (DOMElement *) dnl->item (0);
      defaultServantLibrary = XMLString::transcode 
        (dnDefaultServant->getAttribute( XMLString::transcode ("library")));
      defaultServantFactory = XMLString::transcode 
        (dnDefaultServant->getAttribute( XMLString::transcode ("factory")));
    }

  //------------------------------
  // Policies
  //------------------------------
  // Lifespan
  dnl = deService->getElementsByTagName ( XMLString::transcode ("Lifespan"));
  DOMElement *dnLifespan = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnLifespan->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("TRANSIENT")) == 0)
    {
      lifespan = rootPOA->create_lifespan_policy (PortableServer::TRANSIENT);
    }
  else
    {
      lifespan = rootPOA->create_lifespan_policy (PortableServer::PERSISTENT);
    }

  // IdAssignment
  dnl = deService->getElementsByTagName ( XMLString::transcode ("IdAssignment"));
  DOMElement *dnIdAssignment = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnIdAssignment->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("SYSTEM_ID")) == 0)
    {
      idAssignment = rootPOA->create_id_assignment_policy 
        (PortableServer::SYSTEM_ID);
    }
  else
    {
      idAssignment = rootPOA->create_id_assignment_policy 
        (PortableServer::USER_ID);
    }
    

  // IdUniqueness
  dnl = deService->getElementsByTagName 
    ( XMLString::transcode ("IdUniqueness"));
  DOMElement *dnIdUniqueness = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnIdUniqueness->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("UNIQUE_ID")) == 0)
    {
      idUniqueness = rootPOA->create_id_uniqueness_policy 
        (PortableServer::UNIQUE_ID);
    }
  else
    {
      idUniqueness = rootPOA->create_id_uniqueness_policy
        (PortableServer::MULTIPLE_ID);
    }
    
  // ImplicitActivation
  dnl = deService->getElementsByTagName 
    ( XMLString::transcode ("ImplicitActivation"));
  DOMElement *dnImplicitActivation = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnImplicitActivation->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("NO_IMPLICIT_ACTIVATION")) == 0)
    {
      implicitActivation = rootPOA->create_implicit_activation_policy 
        (PortableServer::NO_IMPLICIT_ACTIVATION);
    }
  else
    {
      implicitActivation = rootPOA->create_implicit_activation_policy
        (PortableServer::IMPLICIT_ACTIVATION);
    }
    
  // RequestProcessing
  // TODO: check that the required libraries and factories have 
  // been specified inthe DepDes
  dnl = deService->getElementsByTagName 
    ( XMLString::transcode ("RequestProcessing"));
  DOMElement *dnRequestProcessing = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnRequestProcessing->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("USE_ACTIVE_OBJECT_MAP_ONLY")) == 0)
    {
      requestProcessing = rootPOA->create_request_processing_policy
        (PortableServer::USE_ACTIVE_OBJECT_MAP_ONLY);
    }
  else if ( XMLString::compareString 
           (dnRequestProcessing->getAttribute( XMLString::transcode ("value")), 
            XMLString::transcode ("USE_DEFAULT_SERVANT")) == 0)
    {
      requestProcessing = rootPOA->create_request_processing_policy
        (PortableServer::USE_DEFAULT_SERVANT);
    }
  else
    {
      requestProcessing = rootPOA->create_request_processing_policy
        (PortableServer::USE_SERVANT_MANAGER);
    }
        
  // ServantRetention
  dnl = deService->getElementsByTagName 
    ( XMLString::transcode ("ServantRetention"));
  DOMElement *dnServantRetention = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnServantRetention->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("RETAIN")) == 0)
    {
      servantRetention = rootPOA->create_servant_retention_policy
        (PortableServer::RETAIN);
    }
  else 
    {
      servantRetention = rootPOA->create_servant_retention_policy
        (PortableServer::NON_RETAIN);
    }
    
  // Thread
  dnl = deService->getElementsByTagName ( XMLString::transcode ("Thread"));
  DOMElement *dnThread = (DOMElement *) dnl->item (0);

  if ( XMLString::compareString 
      (dnThread->getAttribute( XMLString::transcode ("value")), 
       XMLString::transcode ("ORB_CTRL_MODEL")) == 0)
    {
      thread = rootPOA->create_thread_policy
        (PortableServer::ORB_CTRL_MODEL);
    }
  else 
    {
      thread = rootPOA->create_thread_policy
        (PortableServer::SINGLE_THREAD_MODEL);
    }


  CORBA::PolicyList *policyList = new CORBA::PolicyList();
  policyList->length (POLICY_COUNT);
  (*policyList)[LIFESPAN_INDEX]             = lifespan;
  (*policyList)[IDASSIGNMENT_INDEX]         = idAssignment;
  (*policyList)[IDUNIQUENESS_INDEX]         = idUniqueness;
  (*policyList)[IMPLICITACTIVATION_INDEX]   = implicitActivation;
  (*policyList)[REQUESTPROCESSING_INDEX]    = requestProcessing;
  (*policyList)[SERVANTRETENTION_INDEX]     = servantRetention;
  (*policyList)[THREAD_INDEX]               = thread;

  
  //------------------------------
  // Create DeploymentDescriptor
  //------------------------------
  DeploymentDescriptor *deploymentDescriptor = new DeploymentDescriptor 
    (serviceId,
     repositoryId,
     description,
     servantLibrary,
     servantFactory,
     servantManagerLibrary,
     servantManagerFactory,
     defaultServantLibrary,
     defaultServantFactory,
     policyList);

  delete (repositoryId);
  delete (description);
  delete (servantLibrary);
  delete (servantFactory);
  delete (servantManagerLibrary);
  delete (servantManagerFactory);
  delete (defaultServantLibrary);
  delete (defaultServantFactory);

  debug ("DeploymentDescriptorFactory created "
         "deployment descriptor for service: %s",
         serviceId);
   
  return deploymentDescriptor;
}

