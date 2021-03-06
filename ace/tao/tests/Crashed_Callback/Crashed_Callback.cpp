//
// Crashed_Callback.cpp,v 1.2 2001/06/16 02:01:48 kitty Exp
//
#include "Crashed_Callback.h"

ACE_RCSID(Crashed_Callback, Crashed_Callback, "Crashed_Callback.cpp,v 1.2 2001/06/16 02:01:48 kitty Exp")

Crashed_Callback::Crashed_Callback (void)
{
}

CORBA::Boolean
Crashed_Callback::are_you_there (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  return 1;
}

void
Crashed_Callback::crash_now_please (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_DEBUG ((LM_DEBUG,
              "(%P|%t) Crashed_Callback - crashing application \n"));
// Tru64 seems to hang and not abort and dump core when abort() is called
// here. This needs further investigation. This fix is a temporary one.
#if defined (DIGITAL_UNIX) || defined (DEC_CXX)
  ACE_OS::_exit();
#else
  ACE_OS::abort();
#endif

}

void
Crashed_Callback::test_oneway (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
}
