# ------------------------------------------------------------------------
#	term utilities make file
#
#	The following programs use the libtcap interface:
#
#		dispkeys		pgm to print out names of key strokes
#		dispterm		pgm to print out contents of a term definition
#		dumpterm		pgm to dump a term database entry
#		playback		pgm to playback a log file
#		saveterm		pgm to save a term file
#		termput			pgm to display terminal value (ala tput)
#
#	The following programs use the curses interface:
#
#		dispblks		pgm to display block letters
#		dispbox			pgm to display boxes in all styles
#		dispchrs		pgm to print out characters
#		dispclrs		pgm to display all possible colors
#		dispsnap		pgm to display a "snapshot" of a window
#
#	The following programs use the curses & libxvt interface
#	(they are not built by default):
#
#		ditto			pgm to display input/output in multiple windows
#

# ------------------------------------------------------------------------
# target names
#
TARGET	= \
	dispkeys$(EXE_EXT) \
	dispterm$(EXE_EXT) \
	dumpterm$(EXE_EXT) \
	playback$(EXE_EXT) \
	saveterm$(EXE_EXT) \
	termput$(EXE_EXT) \
	\
	dispblks$(EXE_EXT) \
	dispbox$(EXE_EXT) \
	dispchrs$(EXE_EXT) \
	dispclrs$(EXE_EXT) \
	dispsnap$(EXE_EXT)

# ------------------------------------------------------------------------
# cmd options
#
XVT_DIR	= ../libxvt
CUR_DIR	= ../libwin
TCP_DIR	= ../libtcap
TRM_DIR = ../libterm
SYS_DIR = ../libsys

CINCS	= -I$(XVT_DIR) -I$(CUR_DIR) -I$(TCP_DIR) -I$(TRM_DIR) -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

CUR_LIBS	= \
	../lib/libcurses.$(LIB_EXT)

XVT_LIBS	= \
	../lib/$(LIBXVT).$(LIB_EXT)

# ------------------------------------------------------------------------
# source files
#
CSRC	= \
	${TARGET:$(EXE_EXT)=.c} \
	ditto.c

SRC_LIST	= makefile $(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all :	$(TARGET)

# ------------------------------------------------------------------------
# libtcap-based programs
#
dispkeys$(EXE_EXT) : dispkeys.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispkeys.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

dispterm$(EXE_EXT) : dispterm.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispterm.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

dumpterm$(EXE_EXT) : dumpterm.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dumpterm.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

playback$(EXE_EXT) : playback.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) playback.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

saveterm$(EXE_EXT) : saveterm.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) saveterm.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

termput$(EXE_EXT)  : termput.$(OBJ_EXT)  $(CUR_LIBS)
	$(CC) $(LDFLAGS) termput.$(OBJ_EXT)  $(CUR_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# curses-based programs
#
dispblks$(EXE_EXT) : dispblks.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispblks.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

dispbox$(EXE_EXT)  : dispbox.$(OBJ_EXT)  $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispbox.$(OBJ_EXT)  $(CUR_LIBS) $(OS_LIBS) -o $@

dispchrs$(EXE_EXT) : dispchrs.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispchrs.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

dispclrs$(EXE_EXT) : dispclrs.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispclrs.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

dispsnap$(EXE_EXT) : dispsnap.$(OBJ_EXT) $(CUR_LIBS)
	$(CC) $(LDFLAGS) dispsnap.$(OBJ_EXT) $(CUR_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# curses/xvt-based programs
#
ditto$(EXE_EXT)    : ditto.$(OBJ_EXT)    $(CUR_LIBS) $(XVT_LIBS)
	$(CC) $(LDFLAGS) ditto.$(OBJ_EXT)    $(CUR_LIBS) \
		$(XVT_LIBS) $(X11_LIBS) $(NET_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@[ "$(OS_TYPE)" != "UNIX" ] || $(DELIVER) -u . $(TARGET)
	@[ "$(PLATFORM)" != "cygwin-gcc" ] || $(DELIVER) -u . /bin/cygwin1.dll

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@[ "$(OS_TYPE)" != "UNIX" ] || $(INSTALL) . $(TARGET)
	@[ "$(PLATFORM)" != "cygwin-gcc" ] || $(INSTALL) . /bin/cygwin1.dll

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :
	rm -f $(TARGET)

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
	rm -f *.$(OBJ_EXT) $(TARGET) ditto$(EXE_EXT) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
