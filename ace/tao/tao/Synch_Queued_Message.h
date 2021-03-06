// -*- C++ -*-

//=============================================================================
/**
 *  @file Synch_Queued_Message.h
 *
 *  Synch_Queued_Message.h,v 1.2 2001/04/24 08:02:52 coryan Exp
 *
 *  @author Carlos O'Ryan <coryan@uci.edu>
 */
//=============================================================================

#ifndef TAO_SYNCH_QUEUED_MESSAGE_H
#define TAO_SYNCH_QUEUED_MESSAGE_H
#include "ace/pre.h"

#include "Queued_Message.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

/**
 * @class TAO_Synch_Queued_Message
 *
 * @brief Specialize TAO_Queued_Message for synchronous requests,
 *        i.e. twoways and oneways sent with reliability better than
 *        SYNC_NONE.
 *
 * Reliable requests block the sending thread until the message is
 * sent, likewise, the sending thread must be informed if the
 * connection is closed or the message times out.
 *
 * In contrast oneway (and AMI) requests sent with the SYNC_NONE
 * policy are simple discarded if the connection fails or they
 * timeout.
 *
 * Another important difference is the management of the data buffer:
 * one SYNC_NONE messages the buffer is immediately copied into a
 * newly allocated buffer, and must be deallocated.  Other types of
 * requests use the memory allocated by the sending thread.
 *
 */
class TAO_Export TAO_Synch_Queued_Message : public TAO_Queued_Message
{
public:
  /// Constructor
  /**
   * @param contents The message block chain that must be sent.
   */
  TAO_Synch_Queued_Message (const ACE_Message_Block *contents);

  /// Destructor
  virtual ~TAO_Synch_Queued_Message (void);

  const ACE_Message_Block *current_block (void) const;

  /** Implement the Template Methods from TAO_Queued_Message
   */
  //@{
  virtual size_t message_length (void) const;
  virtual int all_data_sent (void) const;
  virtual void fill_iov (int iovcnt_max, int &iovcnt, iovec iov[]) const;
  virtual void bytes_transferred (size_t &byte_count);
  virtual void destroy (void);
  //@}

private:
  /// The contents of the message.
  /**
   * The message is normally generated by a TAO_OutputCDR stream.  The
   * application marshals the payload, possibly generating a chain of
   * message block connected via the 'cont()' field.
   */
  ACE_Message_Block *contents_;

  /// The current message block
  /**
   * The message may be set in multiple writev() operations.  This
   * point keeps track of the next message to send out.
   */
  ACE_Message_Block *current_block_;
};

#include "ace/post.h"
#endif  /* TAO_QUEUED_MESSAGE_H */
