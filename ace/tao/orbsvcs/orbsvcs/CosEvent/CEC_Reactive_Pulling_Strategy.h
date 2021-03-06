/* -*- C++ -*- */
//=============================================================================
/**
 *  @file   CEC_Reactive_Pulling_Strategy.h
 *
 *  CEC_Reactive_Pulling_Strategy.h,v 1.7 2001/09/18 17:52:23 coryan Exp
 *
 *  @author Carlos O'Ryan (coryan@cs.wustl.edu)
 */
//=============================================================================


#ifndef TAO_CEC_REACTIVE_PULLING_STRATEGY_H
#define TAO_CEC_REACTIVE_PULLING_STRATEGY_H
#include "ace/pre.h"

#include "CEC_Pulling_Strategy.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "orbsvcs/ESF/ESF_Worker.h"
#include "tao/corba.h"

class TAO_CEC_ProxyPullConsumer;
class TAO_CEC_EventChannel;
class TAO_CEC_ConsumerAdmin;
class TAO_CEC_SupplierControl;
class TAO_CEC_Reactive_Pulling_Strategy;

/**
 * @class TAO_CEC_Pulling_Strategy_Adapter
 *
 * @brief Forwards timeout events to the Reactive Pulling Strategy
 *
 * The Reactive Pulling Strategy strategy uses the reactor to
 * periodically wakeup and try top pull events from each
 * PullSupplier connected to the EventChannel.
 */
class TAO_Event_Export TAO_CEC_Pulling_Strategy_Adapter : public ACE_Event_Handler
{
public:
  /// Constructor
  TAO_CEC_Pulling_Strategy_Adapter (TAO_CEC_Reactive_Pulling_Strategy *adaptee);

  // = Documented in ACE_Event_Handler.
  virtual int handle_timeout (const ACE_Time_Value &tv,
                              const void *arg = 0);

private:
  /// The adapted object
  TAO_CEC_Reactive_Pulling_Strategy *adaptee_;
};

// ****************************************************************

/**
 * @class TAO_CEC_Reactive_Pulling_Strategy
 *
 * @brief Dispatch using the caller thread.
 *
 * The events are dispatched in FIFO ordering, using the invoking
 * thread to push the event to the consumer.
 */
class TAO_Event_Export TAO_CEC_Reactive_Pulling_Strategy : public TAO_CEC_Pulling_Strategy
{
public:
  /// The scheduler is used to find the range of priorities and similar
  /// info.
  TAO_CEC_Reactive_Pulling_Strategy (const ACE_Time_Value &rate,
                                     TAO_CEC_EventChannel *event_channel,
                                     CORBA::ORB_ptr orb);

  /// Receive the timeout from the adapter
  void handle_timeout (const ACE_Time_Value &tv,
                       const void* arg);

  // = The CEC_Pulling_Strategy methods.
  virtual void activate (void);
  virtual void shutdown (void);

private:
  /// The Adapter for the reactor events
  TAO_CEC_Pulling_Strategy_Adapter adapter_;

  /// The polling rate
  ACE_Time_Value rate_;

  /// The event channel
  TAO_CEC_EventChannel *event_channel_;

  /// The ORB
  CORBA::ORB_var orb_;

  /// To control the timeout policy in the thread
  CORBA::PolicyCurrent_var policy_current_;

  /// Precomputed policy list to the set timeout.
  CORBA::PolicyList policy_list_;

  /// The ORB reactor
  ACE_Reactor *reactor_;
};

// ****************************************************************

class TAO_CEC_Pull_Event : public TAO_ESF_Worker<TAO_CEC_ProxyPullConsumer>
{
public:
  TAO_CEC_Pull_Event (TAO_CEC_ConsumerAdmin *consumer_admin,
                      TAO_CEC_SupplierControl *control);

  virtual void work (TAO_CEC_ProxyPullConsumer *consumer,
                     CORBA::Environment &ACE_TRY_ENV);

private:
  /// Used to propagate the events.
  TAO_CEC_ConsumerAdmin *consumer_admin_;

  /// To report failed or dead suppliers
  TAO_CEC_SupplierControl *supplier_control_;
};

#if defined (__ACE_INLINE__)
#include "CEC_Reactive_Pulling_Strategy.i"
#endif /* __ACE_INLINE__ */

#include "ace/post.h"
#endif /* TAO_CEC_REACTIVE_PULLING_STRATEGY_H */
