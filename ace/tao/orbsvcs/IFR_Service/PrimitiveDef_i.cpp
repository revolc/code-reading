/* -*- C++ -*- */
// PrimitiveDef_i.cpp,v 1.5 2001/03/30 16:43:23 parsons Exp

#include "Repository_i.h"
#include "PrimitiveDef_i.h"

ACE_RCSID(IFR_Service, PrimitiveDef_i, "PrimitiveDef_i.cpp,v 1.5 2001/03/30 16:43:23 parsons Exp")

TAO_PrimitiveDef_i::TAO_PrimitiveDef_i (
    TAO_Repository_i *repo,
    ACE_Configuration_Section_Key section_key
  )
  : TAO_IRObject_i (repo, section_key),
    TAO_IDLType_i (repo, section_key)
{
}

TAO_PrimitiveDef_i::~TAO_PrimitiveDef_i (void)
{
}

CORBA::DefinitionKind
TAO_PrimitiveDef_i::def_kind (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  return CORBA::dk_Primitive;
}

void 
TAO_PrimitiveDef_i::destroy (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  this->destroy_i (ACE_TRY_ENV);
}

void 
TAO_PrimitiveDef_i::destroy_i (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  ACE_THROW (CORBA::BAD_INV_ORDER (2, CORBA::COMPLETED_NO));
}

CORBA::TypeCode_ptr 
TAO_PrimitiveDef_i::type (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_IFR_READ_GUARD_RETURN (CORBA::TypeCode::_nil ());

  return this->type_i (ACE_TRY_ENV);
}

CORBA::TypeCode_ptr 
TAO_PrimitiveDef_i::type_i (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  u_int pkind = 0;
  this->repo_->config ()->get_integer_value (this->section_key_,
                                             "pkind",
                                             pkind);

  switch (ACE_static_cast (CORBA::PrimitiveKind, pkind))
  {
    case CORBA::pk_null:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_null);
    case CORBA::pk_void:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_void);
    case CORBA::pk_short:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_short);
    case CORBA::pk_long:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_long);
    case CORBA::pk_ushort:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_ushort);
    case CORBA::pk_ulong:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_ulong);
    case CORBA::pk_float:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_float);
    case CORBA::pk_double:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_double);
    case CORBA::pk_boolean:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_boolean);
    case CORBA::pk_char:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_char);
    case CORBA::pk_octet:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_octet);
    case CORBA::pk_any:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_any);
    case CORBA::pk_TypeCode:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_TypeCode);
    case CORBA::pk_Principal:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_Principal);
    case CORBA::pk_string:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_string);
    case CORBA::pk_objref:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_Object);
    case CORBA::pk_longlong:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_longlong);
    case CORBA::pk_ulonglong:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_ulonglong);
    case CORBA::pk_longdouble:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_longdouble);
    case CORBA::pk_wchar:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_wchar);
    case CORBA::pk_wstring:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_wstring);
    case CORBA::pk_value_base:
      // TODO - we haven't added this typecode constant yet.
    default:
      return CORBA::TypeCode::_duplicate (CORBA::_tc_null);
  }
}

CORBA::PrimitiveKind 
TAO_PrimitiveDef_i::kind (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_IFR_READ_GUARD_RETURN (CORBA::pk_null);

  return this->kind_i (ACE_TRY_ENV);
}

CORBA::PrimitiveKind 
TAO_PrimitiveDef_i::kind_i (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  u_int pkind = 0;
  this->repo_->config ()->get_integer_value (this->section_key_,
                                             "pkind",
                                             pkind);

  return ACE_static_cast (CORBA::PrimitiveKind, pkind);
}

