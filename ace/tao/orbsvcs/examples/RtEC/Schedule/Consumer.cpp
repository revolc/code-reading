// Consumer.cpp,v 1.1 1999/07/28 18:51:44 coryan Exp

#include "Consumer.h"

ACE_RCSID(EC_Examples, Consumer, "Consumer.cpp,v 1.1 1999/07/28 18:51:44 coryan Exp")

Consumer::Consumer (void)
{
}

void
Consumer::push (const RtecEventComm::EventSet& events,
                CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  if (events.length () == 0)
    {
      ACE_DEBUG ((LM_DEBUG,
                  "Consumer (%P|%t) no events\n"));
      return;
    }

  ACE_DEBUG ((LM_DEBUG, "Consumer (%P|%t) we received event type %d\n",
              events[0].header.type));
}

void
Consumer::disconnect_push_consumer (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
}

// ****************************************************************

#if defined (ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION)
#elif defined(ACE_HAS_TEMPLATE_INSTANTIATION_PRAGMA)
#endif /* ACE_HAS_EXPLICIT_TEMPLATE_INSTANTIATION */
