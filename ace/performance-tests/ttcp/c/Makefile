#----------------------------------------------------------------------------
#       Makefile,v 4.3 1998/09/23 19:01:30 levine Exp
#
#       Makefile for the wrapper-new-ttcp
#----------------------------------------------------------------------------

#----------------------------------------------------------------------------
#       Local macros
#----------------------------------------------------------------------------

BIN     = new-ttcp

LSRC    = $(addsuffix .cpp,$(FILES))
LOBJ    = $(addsuffix .o,$(FILES))

LDLIBS  = $(addprefix .obj/,$(LOBJ)) -Bstatic -Bdynamic

VLDLIBS = $(LDLIBS:%=%$(VAR))

BUILD   = $(VOBJS) $(VBIN)

#----------------------------------------------------------------------------
#       Include macros and targets
#----------------------------------------------------------------------------

include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include $(ACE_ROOT)/include/makeinclude/macros.GNU
include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
include $(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include $(ACE_ROOT)/include/makeinclude/rules.lib.GNU
include $(ACE_ROOT)/include/makeinclude/rules.bin.GNU
include $(ACE_ROOT)/include/makeinclude/rules.local.GNU

#----------------------------------------------------------------------------
#       Local targets
#----------------------------------------------------------------------------

CCFLAGS = -O2 -DLM_RESULTS
#CCFLAGS = -g -DLM_RESULTS
#DCCFLAGS = -g

#----------------------------------------------------------------------------
#       Dependencies
#----------------------------------------------------------------------------

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
