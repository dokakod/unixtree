# ------------------------------------------------------------------------
#	program makefile
#
#	This makefile just builds the "main" program, which is very small
#	and links with the "real" program, which is in libprd.
#
#	For the windows build, we also build and link with a resource file
#	so we can embed an icon in the program for explorer to display.
#

# ------------------------------------------------------------------------
# target names
#
TARGET	= \
	$(PROGRAM)$(EXE_EXT) \
	$(PROGRAM)x$(EXE_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS)
LFLAGS	= $(LOPTS) $(LDEFS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

# ------------------------------------------------------------------------
# list of libraries to link with
#
OSD_LIB	= ../lib/libos.$(LIB_EXT)
CUR_LIB	= ../lib/libcurses.$(LIB_EXT)
UTL_LIB	= ../lib/libutils.$(LIB_EXT)
RES_LIB	= ../lib/libres.$(LIB_EXT)
LNG_LIB	= ../lib/libres-$(RESLANG).$(LIB_EXT)
FTP_LIB	= ../lib/libftp.$(LIB_EXT)
HLP_LIB	= ../lib/libhelp.$(LIB_EXT)
PRD_LIB	= ../lib/libprd.$(LIB_EXT)
ZIP_LIB	= ../lib/libgzip.$(LIB_EXT)
MGC_LIB	= ../lib/libmagic.$(LIB_EXT)
DIF_LIB	= ../lib/libdiff.$(LIB_EXT)

LINK_LIBS	= \
	$(PRD_LIB) \
	$(RES_LIB) \
	$(LNG_LIB) \
	$(DIF_LIB) \
	$(ZIP_LIB) \
	$(MGC_LIB) \
	$(FTP_LIB) \
	$(UTL_LIB) \
	$(OSD_LIB) \
	$(CUR_LIB)

DLINK_LIBS	= \
	$(LINK_LIBS) \
	../lib/libxvt-dummy.$(LIB_EXT)

XLINK_LIBS	= \
	$(LINK_LIBS) \
	../lib/$(LIBXVT).$(LIB_EXT)

# ------------------------------------------------------------------------
# source files
#
RES_FILES	= \
	pgm.rc \
	pgm.ico \
	resource.h

SRC_LIST	= makefile main.c $(RES_FILES)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

# ------------------------------------------------------------------------
# unix executable (without XVT)
#
$(PROGRAM) : main.$(OBJ_EXT) $(DLINK_LIBS)
	$(CC) $(LDFLAGS) main.$(OBJ_EXT) \
		$(DLINK_LIBS) $(NET_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# unix executable (with XVT)
#
$(PROGRAM)x : main.$(OBJ_EXT) $(XLINK_LIBS)
	$(CC) $(LDFLAGS) main.$(OBJ_EXT) \
		$(XLINK_LIBS) $(X11_LIBS) $(NET_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# windows executable
#
$(PROGRAM).exe : main.$(OBJ_EXT) pgm.res $(DLINK_LIBS)
	$(CC) $(LDFLAGS) main.$(OBJ_EXT) pgm.res \
		$(DLINK_LIBS) $(NET_LIBS) $(OS_LIBS) -o $@

$(PROGRAM)x.exe : main.$(OBJ_EXT) pgm.res $(DLINK_LIBS)
	$(CC) $(LDFLAGS) main.$(OBJ_EXT) pgm.res \
		$(DLINK_LIBS) $(NET_LIBS) $(OS_LIBS) -o $@

pgm.res : pgm.rc
	rc $?

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER) . $(TARGET)
	@[ "$(PLATFORM)" != "cygwin-gcc" ] || $(DELIVER) . /bin/cygwin1.dll

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@$(INSTALL) . $(TARGET)
	@[ "$(PLATFORM)" != "cygwin-gcc" ] || $(INSTALL) . /bin/cygwin1.dll

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :
	rm -f $(TARGET)

# ------------------------------------------------------------------------
# lint all source files
#
lint : main.c
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
	rm -f *.$(OBJ_EXT) *.res $(TARGET) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
