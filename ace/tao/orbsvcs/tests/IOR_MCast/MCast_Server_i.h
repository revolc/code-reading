// MCast_Server_i.h,v 1.2 2001/09/20 18:25:27 irfan Exp

#ifndef MCAST_SERVER_I_H
#define MCAST_SERVER_I_H

#include "MCastS.h"

class MCast_Server_i : public POA_MCast::Server {
public:
  MCast_Server_i ();

  // Simple method just to see that the client contacted the
  // server. If does contact, a '0' is returned.
  CORBA::Boolean connect_server (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException));
};

#endif /* MCAST_SERVER_I_H */
