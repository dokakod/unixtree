# ------------------------------------------------------------------------
#	makefile for libxpm
#
#	Targets:
#		libxpm		XPM library
#
#	This library was lifted from the XFree86 source: xc/extras/Xpm/lib
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libxpm
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CINCS	= $(X11_INCS)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	XpmI.h \
	rgbtab.h \
	xpm.h

OBJS	= \
	Attrib.$(OBJ_EXT) \
	CrBufFrI.$(OBJ_EXT) \
	CrBufFrP.$(OBJ_EXT) \
	CrDatFrI.$(OBJ_EXT) \
	CrDatFrP.$(OBJ_EXT) \
	CrIFrBuf.$(OBJ_EXT) \
	CrIFrDat.$(OBJ_EXT) \
	CrIFrP.$(OBJ_EXT) \
	CrPFrBuf.$(OBJ_EXT) \
	CrPFrDat.$(OBJ_EXT) \
	CrPFrI.$(OBJ_EXT) \
	Image.$(OBJ_EXT) \
	Info.$(OBJ_EXT) \
	RdFToBuf.$(OBJ_EXT) \
	RdFToDat.$(OBJ_EXT) \
	RdFToI.$(OBJ_EXT) \
	RdFToP.$(OBJ_EXT) \
	WrFFrBuf.$(OBJ_EXT) \
	WrFFrDat.$(OBJ_EXT) \
	WrFFrI.$(OBJ_EXT) \
	WrFFrP.$(OBJ_EXT) \
	create.$(OBJ_EXT) \
	data.$(OBJ_EXT) \
	hashtab.$(OBJ_EXT) \
	misc.$(OBJ_EXT) \
	parse.$(OBJ_EXT) \
	rgb.$(OBJ_EXT) \
	scan.$(OBJ_EXT)

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
	$(DELIVER) -w lib $(TARGET)

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
