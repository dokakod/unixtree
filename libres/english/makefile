# ------------------------------------------------------------------------
#	makefile for the $LANGUAGE resource file
#
#	This makefile produces the following:
#
#		libres-$LANGUAGE	library for the $LANGUAGE resource arrays
#		$LANGUAGE.res		$LANGUAGE resource file
#
#	and the following program & header files:
#
#		name		type	use
#		------		----	---------------------------------
#		cmds.[ch]	key		command keys
#		dflt.[ch]	str		strings used in config processing
#		errs.[ch]	str		error messages
#		msgs.[ch]	str		generic message strings
#		pgmi.[ch]	str		program-info strings
#

# ------------------------------------------------------------------------
# target to build
#
LANGUAGE	= english

LANG_LIB	= $(LIB_TGT)/libres-$(LANGUAGE).$(LIB_EXT)
LANG_RES	= $(LANGUAGE).res

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR		= ../../libsys
CUR_DIR		= ../../libecurses
RES_DIR		= ..

CINCS		= -I$(RES_DIR) -I$(CUR_DIR) -I$(SYS_DIR)
LINK_LIBS	= $(RES_LIB) $(CUR_LIB) $(SYS_LIB)

CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

# ------------------------------------------------------------------------
# resource file sources
#
RES_OBJS 	= \
	cmds.$(OBJ_EXT) \
	dflt.$(OBJ_EXT) \
	errs.$(OBJ_EXT) \
	msgs.$(OBJ_EXT) \
	pgmi.$(OBJ_EXT)

RES_SRCS	= ${RES_OBJS:.$(OBJ_EXT)=.c}
RES_HDRS	= ${RES_OBJS:.$(OBJ_EXT)=.h}

RES_TEXT	= \
	cmds.txt \
	dflt.txt \
	errs.txt \
	msgs.txt \
	prog.txt \
	resfile.txt

RES_LIST	= \
	cmds.txt \
	dflt.txt \
	errs.txt \
	msgs.txt \
	pgmi.txt \
	resfile.txt

SRC_LIST	= makefile $(RES_TEXT)

# ------------------------------------------------------------------------
# build targets
#
all : $(LANG_LIB) $(LANG_RES)

$(LANG_LIB) : resfile.$(OBJ_EXT) $(RES_OBJS)
	$(MAKELIB) $@ $?

pgmi.txt : prog.txt $(SRC_DIR)/pgminfo $(SRC_DIR)/bldinfo
	$(TEXTSUB) prog.txt >$@

$(RES_SRCS) resfile.c : $(RES_LIST)
	../resmkch $(LANGUAGE)

$(LANG_RES) : $(RES_LIST)
	../restool -c $(LANGUAGE)

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER) . $(LANG_RES)

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@$(INSTALL) . $(LANG_RES)

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@$(LISTSRC) $(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	-rm -f *.$(OBJ_EXT) $(LANG_LIB) $(LANG_RES) pgmi.txt $(OS_CLEAN)
	-rm -f $(RES_SRCS) $(RES_HDRS) resfile.c resfile.h

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
