
#ident "$Id$"

#include "timeC.h"
#include "orbsvcs/CosNamingC.h"
#include <iostream>
#include <iomanip>

int
main (int argc, char *argv[])
{
  try 
    {
      // Initialize the orb:
      CORBA::ORB_var orb = CORBA::ORB_init (argc, argv);
      CORBA::Object_var obj;

      if (argc < 3)
        {
          cout << "Usage: " << argv[0];
          cout << "[-i stringified_ref | -n name]" << endl;
          exit (1);
        }

      if (argv[1][1] ==  'n')
        {
          // Get reference to NameService
          CosNaming::NamingContext_var inc = CosNaming::NamingContext::_narrow 
            (orb->resolve_initial_references ("NameService"));
          
          // Obtain a reference to the "time" object using the NameService.
          CosNaming::Name name;
          name.length (1);
          name[0].id = CORBA::string_dup (argv[2]);
          name[0].kind = CORBA::string_dup ("");
          obj = inc->resolve (name);
        }
      else if (argv[1][1] == 'i')
        {
          obj = orb->string_to_object (argv[2]);
        }
      else
        {
          cout << "Usage: " << argv[0];
          cout << " [-i stringified_ref | -n name]" << endl;
          exit (1);
        }
      
      // Narrow CORBA object reference:
      // tm: reference to Time proxy object
      Time_var tm = Time::_narrow (obj);

      // obj can now be released since _narrow() creates a new proxy
      // object for the narrower interface.
      if (CORBA::is_nil (tm.in ()))
        {
          // Not necessary but consistent:
          // CORBA::release (obj);
          cerr << "Argument is not a time reference" << endl;
          throw 0;
        }

      int hitsPerSecond = 0;
      int sampleSecond = 60;
      int seconds = 0;

      while (seconds < 10)
        {
          try 
            {
              // Get time 
              TimeOfDay tod = tm->get_time ();

              // data logging
              if (sampleSecond != tod.second)
                {
                  cout << hitsPerSecond << endl;
                  sampleSecond = tod.second;
                  hitsPerSecond = 0;
                  seconds++;
                }
              else
                {
                  hitsPerSecond++;
                }
              
//               cout << "Time in " << argv[2] << " is "
//                    << setw (2) << setfill ('0') << tod.hour << ":"
//                    << setw (2) << setfill ('0') << tod.minute << ":"
//                    << setw (2) << setfill ('0') << tod.second << endl;
            }
            catch (const CORBA::Exception &exp)
              {
                cerr << "Caught CORBA exception: " << exp << ". Retrying." << endl;
              }

          //sleep (1);
        }
          
      CORBA::release (orb);
    }
  catch (const CORBA::Exception &exp)
    {
      cerr << "Uncaught CORBA exception: " << exp << endl;
      return 1;
    }
  catch (...)
    {
      return 1;
    }
  
  return 0;
}
