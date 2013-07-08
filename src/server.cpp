
#ident "$Id$"

#include <unistd.h>
#include <tcl.h>

#include "tao/PortableServer/PortableServer.h"
#include "Exception.h"
#include "Log.h"
#include "ORBManager.h"
#include "ServiceManager.h"
#include "NameServiceManager.h"

using namespace std;


//--------------------------------------------------------------------------
// Tcl application commands
//--------------------------------------------------------------------------
#define CREATE_SERVICE_CMD          "service_create"
#define DESTROY_SERVICE_CMD         "service_destroy"
#define GET_SERVICE_STATE_CMD       "service_get_state"
#define SET_SERVICE_STATE_CMD       "service_set_state"
#define ACTIVATE_OBJECT_CMD         "object_activate"
#define DEACTIVATE_OBJECT_CMD       "object_deactivate"
#define BIND_OBJECT_CMD             "name_bind"
#define REBIND_OBJECT_CMD           "name_rebind"
#define UNBIND_OBJECT_CMD           "name_unbind"

//--------------------------------------------------------------------------
// startService:
//--------------------------------------------------------------------------
int createService (ClientData clientData, 
                   Tcl_Interp *interp,
                   int argc,
                   const char *argv[])
{
  if (argc != 2)
    {
      sprintf (interp->result, "Usage: %s serviceId", CREATE_SERVICE_CMD);
      return TCL_ERROR;
    }

  const char *serviceId = argv[1];
  
  try 
    {
      ServiceManager::instance()->createService (serviceId);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }

  sprintf (interp->result, "%s", serviceId);

  return TCL_OK;
}

//--------------------------------------------------------------------------
// destroyService:
//--------------------------------------------------------------------------
int destroyService (ClientData clientData, 
                    Tcl_Interp *interp,
                    int argc,
                    const char *argv[])
{
  if (argc != 2)
    {
      sprintf (interp->result, "Usage: %s serviceId", DESTROY_SERVICE_CMD);
      return TCL_ERROR;
    }

  const char *serviceId = argv[1];
  
  try 
    {
      ServiceManager::instance()->destroyService (serviceId);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }

  sprintf (interp->result, "%s", serviceId);

  return TCL_OK;
}

//--------------------------------------------------------------------------
// getServiceStatus
//--------------------------------------------------------------------------
int getServiceState (ClientData clientData, 
                     Tcl_Interp *interp,
                     int argc,
                     const char *argv[])
{
  if (argc != 2)
    {
      sprintf (interp->result, "Usage: %s serviceId", GET_SERVICE_STATE_CMD);
      return TCL_ERROR;
    }

  const char *serviceId = argv[1];

  try 
    {
      PortableServer::POAManager::State state 
        = ServiceManager::instance()->getServiceState (serviceId);

      switch (state)
        {
        case PortableServer::POAManager::HOLDING:
          Tcl_SetResult (interp, "HOLDING", TCL_STATIC);
          break;
          
        case PortableServer::POAManager::ACTIVE:
          Tcl_SetResult (interp, "HOLDING", TCL_STATIC);
          break;
          
        case PortableServer::POAManager::DISCARDING:
          Tcl_SetResult (interp, "HOLDING", TCL_STATIC);
          break;

        case PortableServer::POAManager::INACTIVE:
          Tcl_SetResult (interp, "HOLDING", TCL_STATIC);
          break;
        }
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }

  return TCL_OK;
}

//--------------------------------------------------------------------------
// setServiceStatus:
//--------------------------------------------------------------------------
int setServiceState (ClientData clientData, 
                     Tcl_Interp *interp,
                     int argc,
                     const char *argv[])
{
  const char usageMessage[] = 
    "Usage: %s serviceId [HOLDING|ACTIVE|DISCARDING]";
  
  if (argc != 3)
    {
      sprintf (interp->result, 
               usageMessage, 
               SET_SERVICE_STATE_CMD);
      return TCL_ERROR;
    }

  const char *serviceId = argv[1];
  const char *stateStr = argv[2];

  try 
    {
      PortableServer::POAManager::State state;

      if (strcmp (stateStr, "HOLDING") == 0)
        {
          state = PortableServer::POAManager::HOLDING;
          Tcl_SetResult (interp, "HOLDING", TCL_STATIC);
        }
      else if (strcmp (stateStr, "ACTIVE") == 0)
        {
          state = PortableServer::POAManager::ACTIVE;
          Tcl_SetResult (interp, "ACTIVE", TCL_STATIC);
        }
      else if (strcmp (stateStr, "DISCARDING") == 0)
        {
          state = PortableServer::POAManager::DISCARDING;
          Tcl_SetResult (interp, "DISCARDING", TCL_STATIC);
        }
      else
        {
          sprintf (interp->result, 
                   usageMessage, 
                   SET_SERVICE_STATE_CMD);
          return TCL_ERROR;
        }
      
      ServiceManager *serviceManager = ServiceManager::instance();
      serviceManager->setServiceState (serviceId, state);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }

  return TCL_OK;
}

//--------------------------------------------------------------------------
// activateObject:
//--------------------------------------------------------------------------
int activateObject (ClientData clientData, 
                    Tcl_Interp *interp,
                    int argc,
                    const char *argv[])
{
  if (argc < 2)
    {
      sprintf (interp->result, 
               "Usage: %s serviceId ?objectId?",
               ACTIVATE_OBJECT_CMD);
      return TCL_ERROR;
    }

  const char *serviceId = argv[1];
  const char *objectId = (argc > 2) ? argv[2] : 0;
  
  try 
    {
      CORBA::String_var strObjectReference = 
        ServiceManager::instance()->activateObject (serviceId, objectId);
      Tcl_SetResult (interp, strObjectReference.inout (), TCL_VOLATILE);
      return TCL_OK;
      
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }
}

//--------------------------------------------------------------------------
// deactivateObject:
//--------------------------------------------------------------------------
int deactivateObject (ClientData clientData, 
                      Tcl_Interp *interp,
                      int argc,
                      const char *argv[])
{
  if (argc < 3)
    {
      sprintf (interp->result, 
               "Usage: %s serviceId ?objectId?",
               DEACTIVATE_OBJECT_CMD);
      return TCL_ERROR;
    }

  const char *serviceId = argv[1];
  const char *objectId = argv[2];
  
  try 
    {
      ServiceManager::instance()->deactivateObject (serviceId, 
                                                    objectId);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }
  
  return TCL_OK;
}

//--------------------------------------------------------------------------
// bindObject:
//--------------------------------------------------------------------------
int bindObject (ClientData clientData, 
                Tcl_Interp *interp,
                int argc,
                const char *argv[])
{
  if (argc < 3)
    {
      sprintf (interp->result, 
               "Usage: %s name objectId",
               BIND_OBJECT_CMD);
      return TCL_ERROR;
    }

  const char *name = argv[1];
  const char *objectRef = argv[2];
  
  try 
    {
      NameServiceManager::bind (name, objectRef);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }
  
  return TCL_OK;
}

//--------------------------------------------------------------------------
// rebindObject:
//--------------------------------------------------------------------------
int rebindObject (ClientData clientData, 
                  Tcl_Interp *interp,
                  int argc,
                  const char *argv[])
{
  if (argc < 3)
    {
      sprintf (interp->result, 
               "Usage: %s name objectId",
               REBIND_OBJECT_CMD);
      return TCL_ERROR;
    }

  const char *name = argv[1];
  const char *objectRef = argv[2];
  
  try 
    {
      NameServiceManager::rebind (name, objectRef);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }
  
  return TCL_OK;
}

//--------------------------------------------------------------------------
// rebindObject:
//--------------------------------------------------------------------------
int unbindObject (ClientData clientData, 
                  Tcl_Interp *interp,
                  int argc,
                  const char *argv[])
{
  if (argc < 2)
    {
      sprintf (interp->result, 
               "Usage: %s name",
               UNBIND_OBJECT_CMD);
      return TCL_ERROR;
    }

  const char *name = argv[1];
  
  try 
    {
      NameServiceManager::unbind (name);
    }
  catch (Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      interp->result = exp.getMessage ();
      return TCL_ERROR;
    }
  
  return TCL_OK;
}

//--------------------------------------------------------------------------
// Tcl_AppInit:
//   hook procedure that is invoked by the main programs
//   for Tcl applications such as tclsh and wish.  Its purpose is to
//   allow new Tcl  applications to be created without modifying the main
//   programs provided as part of Tcl and Tk.
//--------------------------------------------------------------------------
int Tcl_AppInit(Tcl_Interp *interp)
{
  // Tcl_Init reads init.tcl from the Tcl script library
  if (Tcl_Init (interp) == TCL_ERROR) 
    {
      return TCL_ERROR;
    }

  // Register application shell commands
  Tcl_CreateCommand (interp, 
                     CREATE_SERVICE_CMD,
                     createService, 
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     GET_SERVICE_STATE_CMD, 
                     getServiceState, 
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     SET_SERVICE_STATE_CMD, 
                     setServiceState, 
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     DESTROY_SERVICE_CMD, 
                     destroyService,
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     ACTIVATE_OBJECT_CMD, 
                     activateObject,
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     DEACTIVATE_OBJECT_CMD, 
                     deactivateObject,
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     BIND_OBJECT_CMD, 
                     bindObject,
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);

  Tcl_CreateCommand (interp, 
                     REBIND_OBJECT_CMD, 
                     rebindObject,
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);
  Tcl_CreateCommand (interp, 
                     UNBIND_OBJECT_CMD, 
                     unbindObject,
                     (ClientData) NULL, 
                     (Tcl_CmdDeleteProc *) NULL);
  return TCL_OK;
}


//--------------------------------------------------------------------------
// exitHandler: 
//   procedure to invoke before exiting the TclMain event loop.Must have an 
//   interface matching the type Tcl_ExitProc.
//--------------------------------------------------------------------------
void tclExitHandler (ClientData clientdata)
{
  debug ("tclExitHandler ()");
  delete (ServiceManager::instance());
}


//--------------------------------------------------------------------------
// Convenience structure used to pass data to the Tcl event loop
// wrapper function TclMainWrapper
//--------------------------------------------------------------------------
struct ThreadData
{
  int argc;
  char **argv;
  ofstream *log;
  Tcl_AppInitProc *appInitProc;
};


//--------------------------------------------------------------------------
// Function adapter that allow us to run Tcl_Main in a separate thread
//--------------------------------------------------------------------------
void *TclMainWrapper (void *data)
{
  ThreadData *threadData = (ThreadData *) data;

  // Logging options are inherited from the parent thread but output
  // stream must be explicitly set in each thread. Logger doesnt take 
  // charge of stream
  ACE_LOG_MSG->msg_ostream (threadData->log, 0); 

  Tcl_Main (threadData->argc, 
            threadData->argv,
            threadData->appInitProc);

  exit (0);
}


//--------------------------------------------------------------------------
// main
//--------------------------------------------------------------------------
int main (int argc, char *argv[])
{
  //----------------------------------------
  // Set up logging
  //----------------------------------------
  ACE_LOG_MSG->open (argv[0], 
                     ACE_Log_Msg::OSTREAM 
                     | ACE_Log_Msg::VERBOSE_LITE
                     & ! ACE_Log_Msg::STDERR);

  const string logFilename = string(argv[0]) + ".log";
  ofstream myostream (logFilename.c_str(), ios::out);

  if (myostream.bad ()) 
    {
      cerr << "Unable to open or create log file " 
           << logFilename 
           << ". Aborting" 
           << endl;
      return 1;
    } 
  
  // Set the output stream. Logger takes charge of ofstream.
  ACE_LOG_MSG->msg_ostream (&myostream, 1);
  debug ("========== Started ==========");

  //----------------------------------------
  // Register exit handlers
  //---------------------------------------- 
  Tcl_CreateExitHandler (tclExitHandler, (ClientData) NULL);

  //----------------------------------------
  // Start event loops
  //----------------------------------------
  try 
    {
      // Initialize the ORB
      ORBManager::init (argc, argv);

      // Create Tcl event loop thread
      pthread_t tclMainThread;
      ThreadData threadData;
      threadData.argc = argc;
      threadData.argv = argv;
      threadData.log = &myostream;
      threadData.appInitProc = Tcl_AppInit;

      int status = pthread_create (&tclMainThread, 
                                   NULL, 
                                   TclMainWrapper, 
                                   (void *) &threadData);
      if (status != 0)
        {
          error ("Failed creating TclMain event loop thread");
          exit (1);
        }
      else
        {
          pthread_detach (tclMainThread);
        }

      // Start the ORB
      debug ("Starting the ORB");
      ORBManager::getORB()->run();
  }
  catch (const CORBA::Exception &exp) 
    {
      error ("Uncaught CORBA exception.");
      return 1;
    }
  catch (const Exception &exp)
    {
      error ("%s:%d %s", exp.getFile (), exp.getLine (), exp.getMessage ());
      return 1;
    }
  catch (...)
    {
      cerr << "Uncaught exception: " << endl;
      return 1;
    }
}
