// -*- C++ -*-
// POAManager.h,v 1.2 2000/09/01 05:06:29 coryan Exp

// ============================================================================
//
// = LIBRARY
//    TAO
//
// = FILENAME
//    POAManager.h
//
// = DESCRIPTION
//     POAManager
//
// = AUTHOR
//     Irfan Pyarali
//
// ============================================================================

#ifndef TAO_POAMANAGER_H
#define TAO_POAMANAGER_H
#include "ace/pre.h"

#include "PortableServerC.h"

#if !defined (ACE_LACKS_PRAGMA_ONCE)
# pragma once
#endif /* ACE_LACKS_PRAGMA_ONCE */

#include "poa_macros.h"

// Local Object
#include "tao/LocalObject.h"

class TAO_POA;
class TAO_Object_Adapter;
// Forward decl.

// This is to remove "inherits via dominance" warnings from MSVC.
// MSVC is being a little too paranoid.
#if defined(_MSC_VER)
#if (_MSC_VER >= 1200)
#pragma warning(push)
#endif /* _MSC_VER >= 1200 */
#pragma warning(disable:4250)
#endif /* _MSC_VER */

class TAO_PortableServer_Export TAO_POA_Manager :
  public PortableServer::POAManager,
  public TAO_Local_RefCounted_Object
{
  friend class TAO_POA;
  friend class TAO_Object_Adapter;

public:

  void activate (CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

#if (TAO_HAS_MINIMUM_POA == 0)

  void hold_requests (CORBA::Boolean wait_for_completion,
                      CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

  void discard_requests (CORBA::Boolean wait_for_completion,
                         CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

  void deactivate (CORBA::Boolean etherealize_objects,
                   CORBA::Boolean wait_for_completion,
                   CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

#endif /* TAO_HAS_MINIMUM_POA == 0 */

  PortableServer::POAManager::State get_state (CORBA_Environment &ACE_TRY_ENV = TAO_default_environment ())
    ACE_THROW_SPEC ((CORBA::SystemException));

  TAO_POA_Manager (TAO_Object_Adapter &object_adapter);

  ~TAO_POA_Manager (void);

protected:

  void activate_i (CORBA_Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

  void deactivate_i (CORBA::Boolean etherealize_objects,
                     CORBA::Boolean wait_for_completion,
                     CORBA_Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

#if (TAO_HAS_MINIMUM_POA == 0)

  void hold_requests_i (CORBA::Boolean wait_for_completion,
                        CORBA_Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

  void discard_requests_i (CORBA::Boolean wait_for_completion,
                           CORBA_Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException,
                     PortableServer::POAManager::AdapterInactive));

#endif /* TAO_HAS_MINIMUM_POA == 0 */

  PortableServer::POAManager::State get_state_i ()
    ACE_THROW_SPEC ((CORBA::SystemException));

  ACE_Lock &lock (void);

  int remove_poa (TAO_POA *poa);

  int register_poa (TAO_POA *poa);

  PortableServer::POAManager::State state_;

  ACE_Lock &lock_;

  typedef ACE_Unbounded_Set<TAO_POA *> POA_COLLECTION;

  POA_COLLECTION poa_collection_;

  TAO_Object_Adapter &object_adapter_;
};

#if defined (__ACE_INLINE__)
# include "POAManager.i"
#endif /* __ACE_INLINE__ */

#include "ace/post.h"
#endif /* TAO_POAMANAGER_H */
