# ------------------------------------------------------------------------
#	makefile for libgzip
#
#	Targets:
#		libgzip		gzip library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libgzip
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CINCS	= -I../libsys
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	deflate.h \
	infblock.h \
	infcodes.h \
	inffast.h \
	inftrees.h \
	infutil.h \
	zconf.h \
	zlib.h \
	zutil.h \
	\
	libgzip.h

OBJS = \
	adler32.$(OBJ_EXT) \
	compress.$(OBJ_EXT) \
	crc32.$(OBJ_EXT) \
	deflate.$(OBJ_EXT) \
	gzio.$(OBJ_EXT) \
	gzip.$(OBJ_EXT) \
	infblock.$(OBJ_EXT) \
	infcodes.$(OBJ_EXT) \
	inffast.$(OBJ_EXT) \
	inflate.$(OBJ_EXT) \
	inftrees.$(OBJ_EXT) \
	infutil.$(OBJ_EXT) \
	trees.$(OBJ_EXT) \
	uncompr.$(OBJ_EXT) \
	zutil.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile $(HDRS) $(CSRC) \
	gzmain.c

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# build gzip
#
LIBS	= \
	$(LIB_DIR)/libsys.$(LIB_EXT)

gzip$(EXE_EXT) : $(TARGET) gzmain.$(OBJ_EXT) $(LIBS)
	$(CC) $(LDFLAGS) gzmain.$(OBJ_EXT) $(TARGET) $(LIBS) -o $@

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
	rm -f gzip$(EXE_EXT)

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
	rm -f *.$(OBJ_EXT) $(TARGET) gzip$(EXE_EXT) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
