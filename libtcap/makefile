# ------------------------------------------------------------------------
#	termcap make file
#
#	Targets:
#		libtcap		termcap library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libtcap
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR	= ../libsys
TRM_DIR	= ../libterm

CINCS	= -I$(TRM_DIR) -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	libtcap.h \
	tcapcommon.h \
	tcapdata.h \
	tcapdefs.h \
	tcapexec.h \
	tcapkeys.h \
	tcapprt.h \
	tcaprtns.h \
	tcapvals.h \
	tcapvars.h \
	tcapdb.h \
	tcasync.h \
	tcattrs.h \
	tcbeep.h \
	tcblnkscr.h \
	tccinit.h \
	tccolor.h \
	tcdata.h \
	tcdevout.h \
	tcerrmsg.h \
	tcfkeys.h \
	tcfndpth.h \
	tcintio.h \
	tckeyinp.h \
	tcmode.h \
	tcmouse.h \
	tcputtrm.h \
	tcscrncmds.h \
	tcscrndata.h \
	tcscrout.h \
	tcsetterm.h \
	tcsig.h \
	tcsignal.h \
	tctermcap.h \
	tcterminal.h \
	tcterminfo.h \
	tctermdef.h \
	tctrmfile.h \
	tctrmio.h \
	tctrmstr.h \
	tctrmtbls.h \
	tcwinsiz.h

OBJS	= \
	tcapdata.$(OBJ_EXT) \
	tcapexec.$(OBJ_EXT) \
	tcapkeys.$(OBJ_EXT) \
	tcapprt.$(OBJ_EXT) \
	tcapvals.$(OBJ_EXT) \
	tcapvars.$(OBJ_EXT) \
	tcapdb.$(OBJ_EXT) \
	tcaprtns.$(OBJ_EXT) \
	tcasync.$(OBJ_EXT) \
	tcattrbuf.$(OBJ_EXT) \
	tcattrs.$(OBJ_EXT) \
	tcbeep.$(OBJ_EXT) \
	tcblnkscr.$(OBJ_EXT) \
	tccinit.$(OBJ_EXT) \
	tccolor.$(OBJ_EXT) \
	tcdata.$(OBJ_EXT) \
	tcdevout.$(OBJ_EXT) \
	tcerrmsg.$(OBJ_EXT) \
	tcfkeys.$(OBJ_EXT) \
	tcfndpth.$(OBJ_EXT) \
	tcintio.$(OBJ_EXT) \
	tckeyinp.$(OBJ_EXT) \
	tcmode.$(OBJ_EXT) \
	tcmouse.$(OBJ_EXT) \
	tcputtrm.$(OBJ_EXT) \
	tcscrndata.$(OBJ_EXT) \
	tcscrout.$(OBJ_EXT) \
	tcsetterm.$(OBJ_EXT) \
	tcsig.$(OBJ_EXT) \
	tcsignal.$(OBJ_EXT) \
	tctermcap.$(OBJ_EXT) \
	tcterminal.$(OBJ_EXT) \
	tcterminfo.$(OBJ_EXT) \
	tctermdef.$(OBJ_EXT) \
	tctrmfile.$(OBJ_EXT) \
	tctrmio.$(OBJ_EXT) \
	tctrmstr.$(OBJ_EXT) \
	tctrmtbls.$(OBJ_EXT) \
	tcwinsiz.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile $(HDRS) $(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

$(TARGET) : $(OBJS)
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
lint : $(CSRC)
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
