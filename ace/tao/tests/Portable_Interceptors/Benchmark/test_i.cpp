// test_i.cpp,v 1.5 2000/09/16 17:04:45 nanbor Exp

#include "test_i.h"
#include "marker.h"

ACE_RCSID(Benchmark, test_i, "test_i.cpp,v 1.5 2000/09/16 17:04:45 nanbor Exp")

Secure_Vault_i::Secure_Vault_i (CORBA::ORB_ptr orb)
  : orb_ (CORBA::ORB::_duplicate (orb))
{
}
// ctor

CORBA::Short
Secure_Vault_i::ready (CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
  return 1;
}

void
Secure_Vault_i::authenticate (const char * user,
              CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException,
                     Test_Interceptors::Invalid))
{
  if (ACE_OS::strcmp (user, "root") != 0)
    ACE_THROW (Test_Interceptors::Invalid ());

}

CORBA::Long
Secure_Vault_i::update_records (CORBA::Long,
                const Test_Interceptors::Secure_Vault::Record &,
                CORBA::Environment &)
  ACE_THROW_SPEC ((CORBA::SystemException))
{
return 1;
}

void
Secure_Vault_i::shutdown (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->orb_->shutdown (0, ACE_TRY_ENV);
}
