# ------------------------------------------------------------------------
#	makefile for libos
#
#	Targets:
#		libos		low-level O/S library
#
#	Dependencies:
#		FS_TYPE		required file-system type
#		OS_OPTS		optional O/S defines
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libos
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR	= ../libsys

CINCS	= -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS) $(OS_OPTS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS) $(OS_OPTS)

# ------------------------------------------------------------------------
# source files
#
HSRC	= \
	libos.h \
	oscommon.h \
	osddecmd.h \
	osdfname.h \
	osdirfcts.h \
	osdir.h \
	oseject.h \
	osfcomp.h \
	osfilefcts.h \
	osfname.h \
	osfnmatch.h \
	osfsync.h \
	osgetcwd.h \
	osgmtdst.h \
	oshostnm.h \
	osintfc.h \
	osmktmp.h \
	osmnt.h \
	ospath.h \
	osrdlink.h \
	osstatinfo.h \
	osufname.h \
	osusrgrp.h \
	osutime.h \
	osvfname.h \
	osxfname.h

COBJS = \
	osddecmd.$(OBJ_EXT) \
	osdfname.$(OBJ_EXT) \
	osdirfcts.$(OBJ_EXT) \
	osdir.$(OBJ_EXT) \
	oseject.$(OBJ_EXT) \
	osfcomp.$(OBJ_EXT) \
	osfilefcts.$(OBJ_EXT) \
	osfname.$(OBJ_EXT) \
	osfnmatch.$(OBJ_EXT) \
	osfsync.$(OBJ_EXT) \
	osgetcwd.$(OBJ_EXT) \
	osgmtdst.$(OBJ_EXT) \
	oshostnm.$(OBJ_EXT) \
	osintfc.$(OBJ_EXT) \
	osmktmp.$(OBJ_EXT) \
	ospath.$(OBJ_EXT) \
	osrdlink.$(OBJ_EXT) \
	osstatinfo.$(OBJ_EXT) \
	osufname.$(OBJ_EXT) \
	osusrgrp.$(OBJ_EXT) \
	osutime.$(OBJ_EXT) \
	osvfname.$(OBJ_EXT) \
	osxfname.$(OBJ_EXT)

CSRC	= ${COBJS:.$(OBJ_EXT)=.c}

MSRC	= \
	osmnt-aix.c \
	osmnt-bsd.c \
	osmnt-cygwin.c \
	osmnt-dg.c \
	osmnt-hp.c \
	osmnt-linux.c \
	osmnt-netbsd.c \
	osmnt-osf.c \
	osmnt-posix.c \
	osmnt-sgi.c \
	osmnt-svr3.c \
	osmnt-svr4.c \
	osmnt-win.c

MOBJS	= osmnt-$(FS_TYPE).$(OBJ_EXT)

SRC_LIST	= makefile $(HSRC) $(CSRC) $(MSRC)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

$(TARGET) : $(COBJS) $(MOBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all

# ------------------------------------------------------------------------
# targets to be installed
#
install : all

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :

# ------------------------------------------------------------------------
# lint all source files
#
lint : $(CSRC) osmnt-$(FS_TYPE).c
	$(LINT) $(LFLAGS) $?

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	rm -f *.$(OBJ_EXT) $(TARGET) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@

# ------------------------------------------------------------------------
# build osrdlink.c in Windows as a C++ file
#
osrdlink.obj : osrdlink.c
	$(CC) -c $(CFLAGS) /Tposrdlink.c
