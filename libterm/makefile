# ------------------------------------------------------------------------
#	libterm make file
#
#	Targets:
#		libterm		low-level termio library
#
#	Dependencies:
#		TERM_OPTS	optional defines/flags

# ------------------------------------------------------------------------
# target names
#
NAME	= libterm
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR	= ../libsys

CINCS	= -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS) $(TERM_OPTS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS) $(TERM_OPTS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	libterm.h \
	termalarm.h \
	termattrs.h \
	termclock.h \
	termcommon.h \
	termexec.h \
	termexst.h \
	termkbd.h \
	termkeys.h \
	termmode.h \
	termmse.h \
	termname.h \
	termnap.h \
	termpipe.h \
	termprt.h \
	termscrn.h \
	termst.h \
	termsys.h \
	termtime.h \
	termwinsz.h

OBJS	= \
	termalarm.$(OBJ_EXT) \
	termclock.$(OBJ_EXT) \
	termexec.$(OBJ_EXT) \
	termexst.$(OBJ_EXT) \
	termkbd.$(OBJ_EXT) \
	termmode.$(OBJ_EXT) \
	termmse.$(OBJ_EXT) \
	termname.$(OBJ_EXT) \
	termnap.$(OBJ_EXT) \
	termpipe.$(OBJ_EXT) \
	termprt.$(OBJ_EXT) \
	termscrn.$(OBJ_EXT) \
	termtime.$(OBJ_EXT) \
	termwinsz.$(OBJ_EXT)

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
lint :  $(CSRC)
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
