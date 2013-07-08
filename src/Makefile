# $Id$

SHELL				= /bin/sh
BUILD_MODE			= DEBUG

ifneq ($(notdir $(CURDIR)),$(BUILD_MODE))
# were not in the BUILD_MODE subdir, reinvoke ourselves there:
ifeq (0, $(MAKELEVEL))
export VPATH		:= $(CURDIR)
else
override VPATH		:= $(VPATH)/$(notdir $(CURDIR))
endif

SUBDIRS				= 

.SUFFIXES:

.PHONY: clean subdirs $(SUBDIRS)

.DEFAULT all: subdirs
	if [ ! -d $(BUILD_MODE) ]; then mkdir $(BUILD_MODE); fi; \
	$(MAKE) VPATH=$(VPATH) -C $(BUILD_MODE) -f../Makefile $@

subdirs: $(SUBDIRS)

$(SUBDIRS):
	$(MAKE) VPATH=$(VPATH) -C $@

clean:
	-for dir in $(SUBDIRS); do \
		$(MAKE) -C $$dir clean; \
	done 
	-$(RM) -r $(BUILD_MODE)

else
# normal makefile here, with no need to put objects in
# subdirectories, etc.
#
# Also, just add ".." to VPATH and it'll find all the sources
# without any fancy stuff either.

TAO_ROOT		= $(ACE_ROOT)/TAO
CPPFLAGS		= -I. -I$(TAO_ROOT)/orbsvcs
LDFLAGS			+= -L$(ACE_ROOT)/ace -L$(TAO_ROOT)/orbsvcs/orbsvcs
LDLIBS			= -lTAO -lTAO_PortableServer -lTAO_CosNaming -lxerces-c -ltcl
IDL_SRCS		= time
IDL_FILES		= $(addsuffix S, $(IDL_SRCS)) $(addsuffix C, $(IDL_SRCS))
FILES			= $(IDL_FILES)
DEFS			= $(addsuffix .h, $(FILES)) server.h


DEPDES_LIB_SRC	= 	DeploymentDescriptor.cpp		\
					DeploymentDescriptorFactory.cpp \
					ORBManager.cpp					\
					POAFactory.cpp					\
					Exception.cpp					\
					Log.cpp							\
					ServiceManager.cpp				\
					NameServiceManager.cpp			\
					Service.cpp

TIME_LIB_SRC	=   TimeServant.cpp 				\
					TimeDefaultServant.cpp			\
					TimeServantActivator.cpp		\
					TimeServantLocator.cpp			\
					TimeZoneRegistry.cpp

SERVER_SRC		= 	server.cpp testserver.cpp

CLIENT_SRC		=	client.cpp

BIN				= 	libDepDes.so libTime.so server testserver client


#----------------------------------------------------------------------
# Include macros and targets
#---------------------------------------------------------------------- 

include $(ACE_ROOT)/include/makeinclude/wrapper_macros.GNU
include $(ACE_ROOT)/include/makeinclude/macros.GNU
include $(TAO_ROOT)/rules.tao.GNU
include $(ACE_ROOT)/include/makeinclude/rules.common.GNU
include $(ACE_ROOT)/include/makeinclude/rules.nonested.GNU
include $(ACE_ROOT)/include/makeinclude/rules.local.GNU
include $(TAO_ROOT)/taoconfig.mk

# ----------------------------------------------------------------------
# build-mode-specific flags set here
#----------------------------------------------------------------------
ifeq ($(BUILD_MODE), DEBUG)
CFLAGS			+= -g
#LDLIBS			+= -lefence
endif

#----------------------------------------------------------------------
# Local Targets
#----------------------------------------------------------------------

.PRECIOUS: timeC.cpp timeC.i timeC.h
.PRECIOUS: timeS.cpp timeS.i timeS.h
.PRECIOUS: timeS_T.cpp timeS_T.i timeS_T.h


server: server.o
	$(LINK.cc) $(LDFLAGS) -o $@ $^ $(VLDLIBS) \
		-L$(VPATH)/$(BUILD_MODE) -lDepDes $(POSTLINK)	

testserver: testserver.o
	$(LINK.cc) $(LDFLAGS) -o $@ $^ $(VLDLIBS) \
		-L$(VPATH)/$(BUILD_MODE) -lDepDes -lTime $(POSTLINK)	

client: $(addsuffix .o, $(IDL_FILES)) $(CLIENT_SRC:.cpp=.o)
	$(LINK.cc) $(LDFLAGS) -o $@ $^ $(VLDLIBS) $(POSTLINK)

libDepDes.so: $(DEPDES_LIB_SRC:.cpp=.o)
	$(LINK.cc) $(LDFLAGS) -shared -o $@ $^ $(VLDLIBS)

libTime.so: $(addsuffix .o, $(IDL_FILES)) $(TIME_LIB_SRC:.cpp=.o)
	$(LINK.cc) $(LDFLAGS) -shared -o $@ $^


# pattern rule to generate dependancy makefile
%.d: %.cpp
	@echo "Generating dependency for $<"
	set -e; $(CC) -MM -MG $(CPPFLAGS) $< \
		| sed 's/\($*\)\.o[ :]*/\1.o $@ : /g' > $@; \
		[ -s $@ ] || $(RM) $@



# include dependency files containing  rules of the form:
#	<name>.o <name>.d : <source files>
# which fire off the implicit rule to regenerate the 
# dependency makefile if any of the source files have 
# changed.
-include $(DEPDES_LIB_SRC:.cpp=.d) 
-include $(SERVER_SRC:.cpp=.d) 
-include $(TIME_LIB_SRC:.cpp=.d) 
-include $(CLIENT_SRC:.cpp=.d)

# DO NOT DELETE THIS LINE -- g++dep uses it.
# DO NOT PUT ANYTHING AFTER THIS LINE, IT WILL GO AWAY.
endif
