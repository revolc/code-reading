/* -*- C++ -*- */
// EmitsDef_i.cpp,v 1.5 2001/03/30 16:43:15 parsons Exp

#include "Repository_i.h"
#include "EmitsDef_i.h"

ACE_RCSID(IFR_Service, EmitsDef_i, "EmitsDef_i.cpp,v 1.5 2001/03/30 16:43:15 parsons Exp")

TAO_EmitsDef_i::TAO_EmitsDef_i (TAO_Repository_i *repo,
                                ACE_Configuration_Section_Key section_key)
  : TAO_IRObject_i (repo, section_key),
    TAO_Contained_i (repo, section_key),
    TAO_EventDef_i (repo, section_key)
{
}

TAO_EmitsDef_i::~TAO_EmitsDef_i (void)
{
}

CORBA::DefinitionKind
TAO_EmitsDef_i::def_kind (CORBA::Environment &)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  return CORBA::dk_Emits;
}

void 
TAO_EmitsDef_i::destroy (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_IFR_WRITE_GUARD;

  this->destroy_i (ACE_TRY_ENV);
}

void 
TAO_EmitsDef_i::destroy_i (CORBA::Environment & /* ACE_TRY_ENV */)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  // TODO
}

CORBA_Contained::Description *
TAO_EmitsDef_i::describe (CORBA::Environment &ACE_TRY_ENV)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  TAO_IFR_READ_GUARD_RETURN (0);

  return this->describe_i (ACE_TRY_ENV);
}

CORBA_Contained::Description *
TAO_EmitsDef_i::describe_i (CORBA::Environment & /* ACE_TRY_ENV */)
    ACE_THROW_SPEC ((CORBA::SystemException))
{
  // TODO
  return 0;
}

