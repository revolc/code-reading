/* -*- C++ -*- */
// CM_Client.h,v 1.4 1998/10/20 02:34:37 levine Exp

// ============================================================================
//
// = LIBRARY
//    drwho
//
// = FILENAME
//    CM_Client.h
//
// = DESCRIPTION
//    Provides a virtual communcations layer for the client in the
//    drwho program.
//
// = AUTHOR
//    Douglas C. Schmidt
//
// ============================================================================

#ifndef _CM_CLIENT_H
#define _CM_CLIENT_H

#include "Comm_Manager.h"

class CM_Client : public Comm_Manager
{
  // = TITLE
  //   Provides a virtual communcations layer for the client in the
  //   drwho program.
public:
  // = Initialization and termination.
  CM_Client (void);
  // Constructor.

  virtual ~CM_Client (void);
  // Destructor.

  virtual int mux (char *packet, int &packet_length)   = 0;
  virtual int demux (char *packet, int &packet_length) = 0;
  virtual int open (short port_number);
  virtual int receive (int timeout = 0);
  virtual int send (void);

private:
  fd_set read_fd_;
  ACE_Time_Value time_out_;
  ACE_Time_Value *top_;
};

#endif /* _CM_CLIENT_H */
