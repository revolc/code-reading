// client.cpp,v 1.2 2001/03/11 02:15:57 coryan Exp

#include "TestC.h"
#include "ace/Get_Opt.h"

ACE_RCSID(Server_Leaks, client, "client.cpp,v 1.2 2001/03/11 02:15:57 coryan Exp")

const char *ior = "file://test.ior";
int do_shutdown = 0;

int
parse_args (int argc, char *argv[])
{
  ACE_Get_Opt get_opts (argc, argv, "k:x");
  int c;

  while ((c = get_opts ()) != -1)
    switch (c)
      {
      case 'k':
	ior = get_opts.optarg;
	break;

      case 'x':
	do_shutdown = 1;
	break;

      case '?':
      default:
        ACE_ERROR_RETURN ((LM_ERROR,
                           "usage:  %s "
			   "-k <ior> "
			   "-x "
                           "\n",
                           argv [0]),
                          -1);
      }
  // Indicates sucessful parsing of the command line
  return 0;
}

int
main (int argc, char *argv[])
{
  ACE_TRY_NEW_ENV
    {
      CORBA::ORB_var orb =
        CORBA::ORB_init (argc, argv, "", ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (parse_args (argc, argv) != 0)
        return 1;

      CORBA::Object_var tmp =
        orb->string_to_object(ior, ACE_TRY_ENV);
      ACE_TRY_CHECK;

      Test::Ping_Service_var ping_service =
        Test::Ping_Service::_narrow(tmp.in (), ACE_TRY_ENV);
      ACE_TRY_CHECK;

      if (CORBA::is_nil (ping_service.in ()))
        {
          ACE_ERROR_RETURN ((LM_DEBUG,
                             "Nil process reference <%s>\n",
                             ior),
                            1);
        }

      for (int i = 0; i != 10; ++i)
        {
          ping_service->ping (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }
  
      if (do_shutdown)
        {
          ping_service->shutdown (ACE_TRY_ENV);
          ACE_TRY_CHECK;
        }

      orb->destroy (ACE_TRY_ENV);
      ACE_TRY_CHECK;
    }
  ACE_CATCHANY
    {
      ACE_PRINT_EXCEPTION (ACE_ANY_EXCEPTION,
                           "Exception caught:");
      return 1;
    }
  ACE_ENDTRY;

  return 0;
}
