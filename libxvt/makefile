# ------------------------------------------------------------------------
#	libxvt makefile
#
#	We build two versions of this library:
#
#		libxvt			The XVT library with full X11 support
#		libxvt-dummy	The XVT library with dummy routines
#
#	This is to support systems which have no X11 support, or for
#	users who just don't have the X11 development (or run-time) system
#	installed on their system.
#
#	Note that if the env variable LIBXVT is set to "libxvt-dummy",
#	then both versions of the library are the same.
#

# ------------------------------------------------------------------------
# target names
#
PGM_NAME	= pcxterm
LIB_NAME	= libxvt
DMY_NAME	= $(LIB_NAME)-dummy

TARGET_LIBX	= $(LIB_TGT)/$(LIBXVT).$(LIB_EXT)
TARGET_LIBD	= $(LIB_TGT)/$(DMY_NAME).$(LIB_EXT)
TARGET_PGM	= $(PGM_NAME)$(EXE_EXT)

TARGET	= \
	$(TARGET_LIBX) \
	$(TARGET_LIBD) \
	$(TARGET_PGM)

# ------------------------------------------------------------------------
# comment out these lines if you don't want XPM support
#
#XPM_DEFS	= -DUSE_XPM
#XPM_INCS	= -I../libxpm
#XPM_LIBS	= ../lib/libxpm.$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CINCS	= -I../libsys $(X11_INCS) $(XPM_INCS)
CFLAGS	= $(OPTS) $(COPTS) $(CDEFS) $(CINCS) $(XVT_OPTS) $(NET_OPTS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS) $(XVT_OPTS) $(NET_OPTS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

LIBS		= \
	../lib/$(LIBXVT).$(LIB_EXT) \
	$(XPM_LIBS) \
	../lib/libsys.$(LIB_EXT)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	libxvt.h \
	xvtchild.h \
	xvtcolor.h \
	xvtcommon.h \
	xvtcmds.h \
	xvtcurscrn.h \
	xvtdisplay.h \
	xvtemul.h \
	xvtevent.h \
	xvtgcs.h \
	xvtkeys.h \
	xvtlogs.h \
	xvtpipe.h \
	xvtprt.h \
	xvtpty.h \
	xvtrgb.h \
	xvtscrn.h \
	xvtstrings.h \
	xvtterm.h \
	xvttermdefs.h \
	xvttio.h \
	xvttioflgs.h \
	xvttk.h \
	xvtpath.h \
	xvtvt52.h \
	xvtvt100.h \
	xvtx.h

COM_OBJS	= \
	xvtcolor.$(OBJ_EXT) \
	xvtdata.$(OBJ_EXT) \
	xvtmain.$(OBJ_EXT) \
	xvtrgb.$(OBJ_EXT) \
	xvtstrings.$(OBJ_EXT) \
	xvttk.$(OBJ_EXT) \
	xvttkhelp.$(OBJ_EXT) \
	xvttkopts.$(OBJ_EXT) \
	xvttkres.$(OBJ_EXT) \
	xvtpath.$(OBJ_EXT)

DPY_OBJS	= \
	xvtchild.$(OBJ_EXT) \
	xvtcmds.$(OBJ_EXT) \
	xvtcurscrn.$(OBJ_EXT) \
	xvtemul.$(OBJ_EXT) \
	xvtgcs.$(OBJ_EXT) \
	xvtlogs.$(OBJ_EXT) \
	xvtpipe.$(OBJ_EXT) \
	xvtprocs.$(OBJ_EXT) \
	xvtprt.$(OBJ_EXT) \
	xvtpty.$(OBJ_EXT) \
	xvtscrn.$(OBJ_EXT) \
	xvtterm.$(OBJ_EXT) \
	xvttiobld.$(OBJ_EXT) \
	xvttiodmp.$(OBJ_EXT) \
	xvttioflgs.$(OBJ_EXT) \
	xvtvt52cmds.$(OBJ_EXT) \
	xvtvt52keys.$(OBJ_EXT) \
	xvtvt52tbl.$(OBJ_EXT) \
	xvtvt100cmds.$(OBJ_EXT) \
	xvtvt100keys.$(OBJ_EXT) \
	xvtvt100tbl.$(OBJ_EXT)

BLD_OBJS	= \
	xvttermdefs.$(OBJ_EXT)

X11_OBJS	= \
	xvtxcb.$(OBJ_EXT) \
	xvtxclr.$(OBJ_EXT) \
	xvtxdraw.$(OBJ_EXT) \
	xvtxerrs.$(OBJ_EXT) \
	xvtxevent.$(OBJ_EXT) \
	xvtxfont.$(OBJ_EXT) \
	xvtxmc.$(OBJ_EXT) \
	xvtxptr.$(OBJ_EXT) \
	xvtxsb.$(OBJ_EXT) \
	xvtx.$(OBJ_EXT)

DMY_OBJS	= \
	xvtdummy.$(OBJ_EXT)

COM_CSRC	= ${COM_OBJS:.$(OBJ_EXT)=.c}
DPY_CSRC	= ${DPY_OBJS:.$(OBJ_EXT)=.c}
X11_CSRC	= ${X11_OBJS:.$(OBJ_EXT)=.c}
DMY_CSRC	= ${DMY_OBJS:.$(OBJ_EXT)=.c}

CSRC	= $(COM_CSRC) $(DPY_CSRC) $(X11_CSRC) $(DMY_CSRC) main.c textify.c

# ------------------------------------------------------------------------
# list of all source files
#
TERM_FILES	= \
	xvtterm-c.trm \
	xvtterm-m.trm

SRC_LIST = \
	makefile xterm.seq $(TERM_FILES) xvtrgb.txt \
	$(CSRC) $(HDRS)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

# ------------------------------------------------------------------------
# libxvt library
#
$(LIB_TGT)/libxvt.$(LIB_EXT) : $(COM_OBJS) \
	$(DPY_OBJS) $(BLD_OBJS) $(X11_OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# dummy libxvt library
#
$(LIB_TGT)/libxvt-dummy.$(LIB_EXT) : $(COM_OBJS) $(DMY_OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# misc dependencies
#
#xvtrgb.$(OBJ_EXT) : xvtrgb.i

$(X11_OBJS) : xvtx.h

# ------------------------------------------------------------------------
# default color table
#
xvtrgb.i : textify$(EXE_EXT) xvtrgb.txt
	textify -d "!#" -p -s -f -x1 -o $@ \
		-t xvt_rgb_colors -n "default color table" $?

# ------------------------------------------------------------------------
# terminal definition tables
#
xvttermdefs.c : textify$(EXE_EXT) $(TERM_FILES)
	textify -d "#" -f -x8 -o $@ \
		-t xvt_termdefs_c -n "color definitions" -i xvtterm-c.trm \
		-t xvt_termdefs_m -n "mono  definitions" -i xvtterm-m.trm

# ------------------------------------------------------------------------
# program to "textify" a table
#
textify$(EXE_EXT) : textify.$(OBJ_EXT)
	$(CC) $(LDFLAGS) textify.$(OBJ_EXT) \
		../lib/libsys.$(LIB_EXT) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# terminal-emulator program
#
$(TARGET_PGM) : main.$(OBJ_EXT) $(LIBS)
	$(CC) $(LDFLAGS) main.$(OBJ_EXT) $(LIBS) \
		$(X11_LIBS) $(NET_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@[ "$(OS_TYPE)" != "UNIX" ] || \
	{ \
		$(DELIVER) -u . $(TARGET_PGM); \
		$(DELIVER) -w include libxvt.h; \
		$(DELIVER) -w lib $(TARGET_LIBD); \
		$(DELIVER) -w -n libxvt.$(LIB_EXT) lib ../lib/$(LIBXVT).$(LIB_EXT); \
		$(DELIVER) -w -n $(PGM_NAME)-c.trm trm xvtterm-c.trm; \
		$(DELIVER) -w -n $(PGM_NAME)-m.trm trm xvtterm-m.trm; \
		$(DELIVER)    -n $(PGM_NAME)-c.trm trm xvtterm-c.trm; \
		$(DELIVER)    -n $(PGM_NAME)-m.trm trm xvtterm-m.trm; \
	}

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@[ "$(OS_TYPE)" != "UNIX" ] || \
	{ \
		$(INSTALL) . $(TARGET_PGM); \
		$(INSTALL) -n $(PGM_NAME)-c.trm trm xvtterm-c.trm; \
		$(INSTALL) -n $(PGM_NAME)-m.trm trm xvtterm-m.trm; \
	}

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :
	rm -f $(TARGET_PGM)

# ------------------------------------------------------------------------
# lint all source files
#
lint : $(X11_CSRC)
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
	-rm -f *.$(OBJ_EXT) $(TARGET) textify$(EXE_EXT) xvttermdefs.c xvtrgb.i

# ------------------------------------------------------------------------
# build rules
#
xvtx.$(OBJ_EXT) : xvtx.c
	$(CC) -c $(CFLAGS) $(XPM_DEFS) $<

%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
