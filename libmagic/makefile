# ------------------------------------------------------------------------
#	makefile for libmagic
#
#	Targets:
#		libmagic		magic library
#		file			test program
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libmagic
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR	= ../libsys

CINCS	= -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	libmagic.h \
	magcommon.h

COBJS = \
	magascii.$(OBJ_EXT) \
	magchk.$(OBJ_EXT) \
	magdump.$(OBJ_EXT) \
	magfile.$(OBJ_EXT) \
	magfstat.$(OBJ_EXT) \
	magintl.$(OBJ_EXT) \
	magsoft.$(OBJ_EXT) \
	magtar.$(OBJ_EXT)

CSRC	= ${COBJS:.$(OBJ_EXT)=.c}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile $(HDRS) $(CSRC) \
	magmain.c magic.txt

# ------------------------------------------------------------------------
# build targets
#
all :	$(TARGET)

$(TARGET) : $(COBJS)
	$(MAKELIB) $@ $?

$(COBJS) : libmagic.h

# ------------------------------------------------------------------------
# build "file" (test program)
#
CINCS	= -I../libsys

LIBS	= \
	$(LIB_DIR)/libsys.$(LIB_EXT)

file$(EXE_EXT) : $(TARGET) magmain.$(OBJ_EXT) $(LIBS)
	$(CC) $(LDFLAGS) magmain.$(OBJ_EXT) $(TARGET) $(LIBS) -o $@

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER) -n magic . magic.txt

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@$(INSTALL) -n magic . magic.txt

# ------------------------------------------------------------------------
# delete any linked executables
#
notarget :
	rm -f file$(EXE_EXT)

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
	rm -f *.$(OBJ_EXT) $(TARGET) file$(EXE_EXT) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
