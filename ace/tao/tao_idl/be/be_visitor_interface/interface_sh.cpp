//
// interface_sh.cpp,v 1.38 2001/07/10 17:02:05 parsons Exp
//

// ============================================================================
//
// = LIBRARY
//    TAO IDL
//
// = FILENAME
//    interface_sh.cpp
//
// = DESCRIPTION
//    Visitor generating code for Interfaces in the server header
//
// = AUTHOR
//    Aniruddha Gokhale
//
// ============================================================================

#include        "idl.h"
#include        "idl_extern.h"
#include        "be.h"

#include "be_visitor_interface.h"

ACE_RCSID(be_visitor_interface, interface_sh, "interface_sh.cpp,v 1.38 2001/07/10 17:02:05 parsons Exp")


  // ************************************************************
  // Interface visitor for server header
  // ************************************************************

  be_visitor_interface_sh::be_visitor_interface_sh (be_visitor_context *ctx)
    : be_visitor_interface (ctx)
{
}

be_visitor_interface_sh::~be_visitor_interface_sh (void)
{
}

int
be_visitor_interface_sh::visit_interface (be_interface *node)
{
  TAO_OutStream *os; // output stream
  long i; // loop index
  static char namebuf [NAMEBUFSIZE]; // holds the class name

  if (node->srv_hdr_gen () || node->imported () || node->is_local ())
    return 0;

  ACE_OS::memset (namebuf, '\0', NAMEBUFSIZE);

  os = this->ctx_->stream ();

  // Generate the skeleton class name.

  os->indent ();

  // We shall have a POA_ prefix only if we are at the topmost level.
  if (!node->is_nested ())
    {
      // We are outermost.
      ACE_OS::sprintf (namebuf, "POA_%s", node->local_name ());
    }
  else
    {
      ACE_OS::sprintf (namebuf, "%s", node->local_name ());
    }

  *os << "class " << namebuf << ";" << be_nl;

  // Generate the _ptr declaration.
  *os << "typedef " << namebuf << " *" << namebuf
      << "_ptr;" << be_nl;

  // Forward class declaration.
  *os << "// Forward Classes Declaration" << be_nl;

  if (be_global->gen_thru_poa_collocation ())
    {
      *os << "class " << node->thru_poa_proxy_impl_name () << ";" << be_nl;
    }

  if (be_global->gen_direct_collocation ())
    {
      *os << "class " << node->direct_proxy_impl_name () << ";" << be_nl;
    }

  if (be_global->gen_thru_poa_collocation ()
      || be_global->gen_direct_collocation ())
    {
      *os << "class " << node->strategized_proxy_broker_name () 
          << ";" << be_nl;
    }

  *os << be_nl;

  // Now generate the class definition.
  *os << "class " << be_global->skel_export_macro ()
      << " " << namebuf << be_idt_nl << ": " << be_idt;

  long n_parents = node->n_inherits ();

  if (n_parents > 0)
    {
      for (i = 0; i < n_parents; ++i)
        {
          *os << "public virtual " << "POA_" 
              << node->inherits ()[i]->name ();

          if (i < n_parents - 1)
            {
              *os << "," << be_nl;
            }
        }
    }
  else
    {
      // We don't inherit from another user defined object, hence our
      // base class is the ServantBase class.
      *os << "public virtual PortableServer::ServantBase";
    }

  *os << be_uidt << be_uidt_nl
      << "{" << be_nl
      << "protected:" << be_idt_nl
      << namebuf << " (void);\n" << be_uidt_nl
      << "public:" << be_idt_nl;

  // No copy constructor for locality constraint interface.
  *os << namebuf << " (const " << namebuf << "& rhs);" << be_nl
      << "virtual ~" << namebuf << " (void);\n\n"
      << be_nl
      << "virtual CORBA::Boolean _is_a (" << be_idt << be_idt_nl
      << "const char* logical_type_id," << be_nl
      << "CORBA::Environment &ACE_TRY_ENV = " << be_idt_nl
      << "TAO_default_environment ()"
      << be_uidt << be_uidt_nl
      << ");\n" << be_uidt_nl;

  *os << "virtual void* _downcast (" << be_idt << be_idt_nl
      << "const char* logical_type_id" << be_uidt_nl
      << ");\n" << be_uidt_nl;

  // Add a skeleton for our _is_a method.
  *os << "static void _is_a_skel (" << be_idt << be_idt_nl
      << "TAO_ServerRequest &req," << be_nl
      << "void *obj," << be_nl
      << "void *servant_upcall," << be_nl
      << "CORBA::Environment &ACE_TRY_ENV" << be_uidt_nl
      << ");\n" << be_uidt_nl;

  // Add a skeleton for our _non_existent method.
  *os << "static void _non_existent_skel (" << be_idt << be_idt_nl
      << "TAO_ServerRequest &req," << be_nl
      << "void *obj," << be_nl
      << "void *servant_upcall," << be_nl
      << "CORBA::Environment &ACE_TRY_ENV" << be_uidt_nl
      << ");\n" << be_uidt_nl;

  // Add a skeleton for our _interface method.
  *os << "static void _interface_skel (" << be_idt << be_idt_nl
      << "TAO_ServerRequest &req," << be_nl
      << "void *obj," << be_nl
      << "void *servant_upcall," << be_nl
      << "CORBA::Environment &ACE_TRY_ENV"
      << be_uidt_nl
      << ");\n" << be_uidt_nl;

  // Add the dispatch method.
  *os << "virtual void _dispatch (" << be_idt << be_idt_nl
      << "TAO_ServerRequest &req," << be_nl
      << "void *_servant_upcall," << be_nl
      << "CORBA::Environment &ACE_TRY_ENV" << be_uidt_nl
      << ");\n" << be_uidt_nl;

  this->this_method (node);

  // The _interface_repository_id method.
  *os << "virtual const char* _interface_repository_id "
      << "(void) const;\n\n";

  // Generate code for elements in the scope (e.g., operations).
  if (this->visit_scope (node) ==  -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_sh::"
                         "visit_interface - "
                         "codegen for scope failed\n"),
                        -1);
    }

  // Generate skeletons for operations of our base classes. These
  // skeletons just cast the pointer to the appropriate type
  // before invoking the call.
  if (node->traverse_inheritance_graph (be_interface::gen_skel_helper, os) == -1)
    {
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_sh::"
                         "visit_interface - "
                         "inheritance graph traversal failed\n"),
                        -1);
    }

  // Generate the embedded RequestInfo classes per operation.
  // This is to be used by interceptors.
  be_visitor_context ctx (*this->ctx_);
  be_visitor *visitor = 0;
  // Interceptor related classes.
  ctx.state (TAO_CodeGen::TAO_INTERFACE_INTERCEPTORS_SH);
  visitor = tao_cg->make_visitor (&ctx);

  if (!visitor || (node->accept (visitor) == -1))
    {
      delete visitor;
      ACE_ERROR_RETURN ((LM_ERROR,
                         "be_visitor_interface_ch::"
                         "visit_interface - "
                         "codegen for interceptor classes failed\n"),
                        -1);
    }

  delete visitor;
  visitor = 0;

  *os << be_uidt_nl << "};\n\n";

  if (be_global->gen_thru_poa_collocation () ||
      be_global->gen_direct_collocation ())
    {
      ctx = *this->ctx_;
      // Generate strategized proxy broker.
      ctx.state (TAO_CodeGen::TAO_INTERFACE_STRATEGIZED_PROXY_BROKER_SH);
      visitor = tao_cg->make_visitor (&ctx);
      if (node->accept (visitor) == -1)
	      {
	        delete visitor;
	        ACE_ERROR_RETURN ((LM_ERROR,
			           "be_visitor_interface_sh::"
			           "visit_interface - "
			           "codegen for thru_poa_collocated class failed\n"),
			          -1);
	      }

      delete visitor;
    }

  ctx = *this->ctx_;

  // Generate the collocated class.
  if (be_global->gen_thru_poa_collocation ())
    {
      visitor = 0;
      ctx = *this->ctx_;
      ctx.state (TAO_CodeGen::TAO_INTERFACE_THRU_POA_PROXY_IMPL_SH);
      visitor = tao_cg->make_visitor (&ctx);

      if (visitor == 0 || node->accept (visitor) == -1)
        {
          delete visitor;
          ACE_ERROR_RETURN ((LM_ERROR,
                             "be_visitor_interface_sh::"
                             "visit_interface - "
                             "codegen for thru_poa_collocated class failed\n"),
                            -1);
        }

      delete visitor;
    }

  ctx = *this->ctx_;

  if (be_global->gen_direct_collocation ())
    {
      visitor = 0;
      ctx = *this->ctx_;
      ctx.state (TAO_CodeGen::TAO_INTERFACE_DIRECT_PROXY_IMPL_SH);
      visitor = tao_cg->make_visitor (&ctx);

      if (visitor == 0 || node->accept (visitor) == -1)
        {
          delete visitor;
          ACE_ERROR_RETURN ((LM_ERROR,
                             "be_visitor_interface_sh::"
                             "visit_interface - "
                             "codegen for thru_poa_collocated class failed\n"),
                            -1);
        }

      delete visitor;
    }

  *os << "\n";
  return 0;
}


void
be_visitor_interface_sh::this_method (be_interface *node)
{
  TAO_OutStream *os = this->ctx_->stream ();

  // Print out the _this() method.
  *os << "::" << node->full_name () << " *_this (" << be_idt << be_idt_nl
      << "CORBA::Environment &ACE_TRY_ENV = " << be_idt_nl
      << "TAO_default_environment ()"
      << be_uidt << be_uidt_nl
      << ");\n" << be_uidt_nl;
}
