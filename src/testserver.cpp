
#ident "$Id$"

#include <unistd.h>

#include "tao/corba.h"
#include "orbsvcs/CosNamingC.h"
#include "tao/PortableServer/PortableServer.h"
#include "Exception.h"
#include "Log.h"
#include "TimeServant.h"

using namespace std;


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
  ofstream myostream (logFilename.c_str(), ios::out | ios::app);

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

  try 
    {
      // Initialize the ORB
      CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);

      // Get reference to Root POA
      CORBA::Object_var obj = orb->resolve_initial_references ("RootPOA");
      PortableServer::POA_var poa = PortableServer::POA::_narrow (obj);

      // Get reference to Naming Service
      obj = orb->resolve_initial_references ("NameService");
      CosNaming::NamingContext_var namingContext = 
        CosNaming::NamingContext::_narrow (obj);

      // Activate POA manager
      PortableServer::POAManager_var mgr = poa->the_POAManager ();
      mgr->activate ();

      // Create objects
      TimeServant gambiaServant("Gambia");
      TimeServant zimbabweServant("Zimbabwe");
      TimeServant haitiServant("Haiti");

      // Activate objects
      Time_var gambia = gambiaServant._this ();
      Time_var zimbabwe = zimbabweServant._this ();
      Time_var haiti = haitiServant._this ();
      
      // Name service binding
      CosNaming::Name cosName;
      cosName.length (1);
      cosName[0].kind = CORBA::string_dup ("");
      
      cosName[0].id = CORBA::string_dup ("Gambia");
      namingContext->rebind (cosName, gambia);

      cosName[0].id = CORBA::string_dup ("Zimbabwe");
      namingContext->rebind (cosName, zimbabwe);

      cosName[0].id = CORBA::string_dup ("Haiti");
      namingContext->rebind (cosName, haiti);
      
      // Start the ORB
      debug ("Starting the ORB");
      orb->run();
  }
  catch (const CORBA::Exception &exp) 
    {
      error ("Uncaught CORBA exception.");
      return 1;
    }

  return 0;
}
