
// -*- C++ -*-
// generic_servant_export.h,v 1.3 2000/10/10 22:15:59 nanbor Exp
// Definition for Win32 Export directives.
// This file is generated automatically by generate_export_file.pl
// ------------------------------
#ifndef GENERIC_SERVANT_EXPORT_H
#define GENERIC_SERVANT_EXPORT_H

#include "ace/config-all.h"

#if defined (TAO_AS_STATIC_LIBS)
#  if !defined (GENERIC_SERVANT_HAS_DLL)
#    define GENERIC_SERVANT_HAS_DLL 0
#  endif /* ! GENERIC_SERVANT_HAS_DLL */
#else
#  if !defined (GENERIC_SERVANT_HAS_DLL)
#    define GENERIC_SERVANT_HAS_DLL 1
#  endif /* ! GENERIC_SERVANT_HAS_DLL */
#endif

#if defined (GENERIC_SERVANT_HAS_DLL) && (GENERIC_SERVANT_HAS_DLL == 1)
#  if defined (GENERIC_SERVANT_BUILD_DLL)
#    define GENERIC_SERVANT_Export ACE_Proper_Export_Flag
#    define GENERIC_SERVANT_SINGLETON_DECLARATION(T) ACE_EXPORT_SINGLETON_DECLARATION (T)
#    define GENERIC_SERVANT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_EXPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  else /* GENERIC_SERVANT_BUILD_DLL */
#    define GENERIC_SERVANT_Export ACE_Proper_Import_Flag
#    define GENERIC_SERVANT_SINGLETON_DECLARATION(T) ACE_IMPORT_SINGLETON_DECLARATION (T)
#    define GENERIC_SERVANT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK) ACE_IMPORT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#  endif /* GENERIC_SERVANT_BUILD_DLL */
#else /* GENERIC_SERVANT_HAS_DLL == 1 */
#  define GENERIC_SERVANT_Export
#  define GENERIC_SERVANT_SINGLETON_DECLARATION(T)
#  define GENERIC_SERVANT_SINGLETON_DECLARE(SINGLETON_TYPE, CLASS, LOCK)
#endif /* GENERIC_SERVANT_HAS_DLL == 1 */

#endif /* GENERIC_SERVANT_EXPORT_H */

// End of auto generated file.
