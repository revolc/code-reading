// Collocation_Tester.cpp,v 1.2 2000/11/22 23:35:18 corsaro Exp

#include "Collocation_Tester.h"

ACE_RCSID (tests, Collocation_Tester, "Collocation_Tester.cpp,v 1.2 2000/11/22 23:35:18 corsaro Exp")

const char *Quote::top = "Ciao";
const char *Quote::left = "Hola";
const char *Quote::right = "Hello";
const char *Quote::bottom = "Previet";

Collocation_Tester::Collocation_Tester (CORBA::Object_ptr object)
{
  this->object_ = CORBA::Object::_duplicate (object);
}

Collocation_Tester::~Collocation_Tester (void)
{
  // No-Op.
}

int
Collocation_Tester::match_answer (const char *actual_answer,
                                  const char *right_answer,
                                  const char *method_name)
{
  if (ACE_OS::strcmp (actual_answer, right_answer))
    {
      ACE_ERROR ((LM_DEBUG,
                  ACE_TEXT ("Call to <%s> returned the wrong value!\n"), method_name));
      return 1;
    }
  else
    {
      ACE_DEBUG ((LM_DEBUG,
                  ACE_TEXT ("Call to <%s> returned with success: %s \n"), method_name, actual_answer));
      return 0;
    }
}

int
Collocation_Tester::test_top (CORBA::Environment &ACE_TRY_ENV)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n------------------------<TOP>-------------------------------\n")));
  int failure = 0;
  int call_num = 1;
  Multiple::Top_var top =
    Multiple::Top::_narrow (this->object_.in ());

  if (CORBA::is_nil (top.in ()))
    {
      ACE_ERROR ((LM_DEBUG,
                  ACE_TEXT ("Unable to Narrow to Multiple::Top\n")));
      return 1;
    }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\n\nCalling all method supported by the Interface Top\n\n")));

  CORBA::String_var msg = top->top_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN (0);

  failure += this->match_answer (Quote::top, msg.in (), "top_quote");

  if (failure)
    ACE_ERROR ((LM_DEBUG,
                ACE_TEXT ("The test had %d/%d Failure\n"), failure, call_num));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n------------------------<TOP>-------------------------------\n")));

  return failure;
}

int
Collocation_Tester::test_right (CORBA::Environment &ACE_TRY_ENV)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n------------------------<RIGHT>-------------------------------\n")));
  int failure = 0;
  int call_num = 2;
  Multiple::Right_var right =
    Multiple::Right::_narrow (this->object_.in ());

  if (CORBA::is_nil (right.in ()))
    {
      ACE_ERROR ((LM_DEBUG,
                  ACE_TEXT ("Unable to Narrow to Multiple::Right\n")));
      return 1;
    }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\n\nCalling all method supported by the Interface Right\n\n")));

  CORBA::String_var msg = right->top_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN (1);

  failure += this->match_answer (Quote::top, msg.in (), "top_quote");

  msg = right->right_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN (1);

  failure += this->match_answer (Quote::right, msg.in (), "right_quote");

  if (failure)
    ACE_ERROR ((LM_DEBUG,
                ACE_TEXT ("The test had %d/%d Failure"), failure, call_num));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n------------------------<RIGHT>-------------------------------\n")));

  return failure;
}


int
Collocation_Tester::test_left (CORBA::Environment &ACE_TRY_ENV)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n------------------------<LEFT>-------------------------------\n")));
  int failure = 0;
  int call_num = 2;

  Multiple::Left_var left =
    Multiple::Left::_narrow (this->object_.in ());

  if (CORBA::is_nil (left.in ()))
    {
      ACE_ERROR ((LM_DEBUG,
                  ACE_TEXT ("Unable to Narrow to Multiple::Right\n")));
      return 1;
    }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\n\nCalling all method supported by the Interface Left\n\n")));

  CORBA::String_var msg = left->top_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN (1);

  failure += this->match_answer (Quote::top, msg.in (), "top_quote");

  msg = left->left_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN(1);

  failure += this->match_answer (Quote::left, msg.in (), "left_quote");

  if (failure)
    ACE_ERROR ((LM_DEBUG,
                ACE_TEXT ("The test had %d/%d Failure"), failure, call_num));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n------------------------<LEFT>-------------------------------\n")));

  return failure;
}


int
Collocation_Tester::test_bottom (CORBA::Environment &ACE_TRY_ENV)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n-----------------------<BOTTOM>-------------------------------\n")));
  int failure = 0;
  int call_num = 4;

  Multiple::Bottom_var bottom =
    Multiple::Bottom::_narrow (this->object_.in ());

  if (CORBA::is_nil (bottom.in ()))
    {
      ACE_ERROR ((LM_DEBUG,
                  ACE_TEXT ("Unable to Narrow to Multiple::Right\n")));
      return 1;
    }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\n\nCalling all method supported by the Interface Bottom\n\n")));

  CORBA::String_var msg = bottom->top_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN(1);

  failure += this->match_answer (Quote::top, msg.in (), "top_quote");

  msg = bottom->left_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN (1);

  failure += this->match_answer (Quote::left, msg.in (), "left_quote");

  msg = bottom->right_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN(1);

  failure += this->match_answer (Quote::right, msg.in (), "right_quote");

  msg = bottom->bottom_quote (ACE_TRY_ENV);
  ACE_CHECK_RETURN(1);

  failure += this->match_answer (Quote::bottom, msg.in (), "bottom_quote");

  if (failure)
    ACE_ERROR ((LM_DEBUG,
                ACE_TEXT ("The test had %d/%d Failure"), failure, call_num));

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT("\n-----------------------<BOTTOM>-------------------------------\n")));

  return failure;
}

void
Collocation_Tester::shutdown (CORBA::Environment &ACE_TRY_ENV)
{
  Multiple::Top_var top =
    Multiple::Top::_narrow (this->object_.in ());

  if (CORBA::is_nil (top.in ()))
    {
      ACE_ERROR ((LM_DEBUG,
                  ACE_TEXT ("Unable to Narrow to Multiple::Top\n")));
      return;
    }

  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT ("\n\nCalling ShutDown on Top.\n\n")));

  top->shutdown (ACE_TRY_ENV);
  ACE_CHECK;
}

void
Collocation_Tester::run (CORBA::Environment &ACE_TRY_ENV)
{
  int failure_num =   this->test_top (ACE_TRY_ENV);
  ACE_CHECK;

  failure_num += this->test_left (ACE_TRY_ENV);
  ACE_CHECK;

  failure_num += this->test_right (ACE_TRY_ENV);
  ACE_CHECK;

  failure_num += this->test_bottom (ACE_TRY_ENV);
  ACE_CHECK;

  if (failure_num)
    ACE_ERROR ((LM_DEBUG,
                ACE_TEXT ("\n\nThe Test failed in %d cases."), failure_num));
  else
    ACE_DEBUG ((LM_DEBUG,
                ACE_TEXT ("\n\nThe Test Completed Successfully. Congratulations! ")));

  this->shutdown (ACE_TRY_ENV);
}
