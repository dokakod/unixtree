# ------------------------------------------------------------------------
#	Utility routines make file
#
#	Targets:
#		libutils		Utility routines library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libutils
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CUR_DIR	= ../libecurses
OSD_DIR	= ../libos
SYS_DIR	= ../libsys

CINCS	= -I$(CUR_DIR) -I$(OSD_DIR) -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	libutils.h \
	utblist.h \
	utcommon.h \
	utcompress.h \
	utdatecvt.h \
	utdefaults.h \
	utfileattr.h \
	utlist.h \
	utmntlist.h \
	utregexp.h \
	utssort.h \
	utstrutils.h \
	uttree.h

OBJS	= \
	utblist.$(OBJ_EXT) \
	utcompress.$(OBJ_EXT) \
	utdatecvt.$(OBJ_EXT) \
	utdefaults.$(OBJ_EXT) \
	utfileattr.$(OBJ_EXT) \
	utitos.$(OBJ_EXT) \
	utlist.$(OBJ_EXT) \
	utmntlist.$(OBJ_EXT) \
	utregexp.$(OBJ_EXT) \
	utssort.$(OBJ_EXT) \
	utstoi.$(OBJ_EXT) \
	utstrccmp.$(OBJ_EXT) \
	utstrings.$(OBJ_EXT) \
	uttree.$(OBJ_EXT)

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
