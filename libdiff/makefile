# ------------------------------------------------------------------------
#	diff routines make file
#
#	Targets:
#		libdiff		diff routines library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libdiff
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
	libdiff.h \
	diffsys.h

OBJS	= \
	diffanal.$(OBJ_EXT) \
	diffeng.$(OBJ_EXT) \
	diffprtd.$(OBJ_EXT) \
	diffprts.$(OBJ_EXT) \
	diffio.$(OBJ_EXT) \
	diffside.$(OBJ_EXT) \
	diffutil.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile $(HDRS) $(CSRC) \
	diffmain.c

# ------------------------------------------------------------------------
# build targets
#
all :	$(TARGET)

$(TARGET) : $(OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# build diff
#
LIBS	= \
	$(LIB_DIR)/libsys.$(LIB_EXT)

diff$(EXE_EXT) : $(TARGET) diffmain.$(OBJ_EXT) $(LIBS)
	$(CC) $(LDFLAGS) diffmain.$(OBJ_EXT) $(TARGET) $(LIBS) -o $@

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
	rm -f diff$(EXE_EXT)

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
	rm -f *.$(OBJ_EXT) $(TARGET) diff$(EXE_EXT) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
