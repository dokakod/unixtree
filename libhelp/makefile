# ------------------------------------------------------------------------
#	makefile for help files
#
#	This makefile does not actually create a library, it just
#	creates the following:
#
#		$(LANGUAGES).hlp	help file(s)
#		libhelp.hlp			include file
#

# ------------------------------------------------------------------------
# languages to build
#
LANGUAGES	= \
	english \
	german

# ------------------------------------------------------------------------
# some makes can't automatically create the following, so we do it manually
#
HELP_FILES	= \
	english.hlp \
	german.hlp

# ------------------------------------------------------------------------
# target names
#
TARGET	= libhelp.h

HLPBLD	= hlpbld$(EXE_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CINCS	= -I../libsys
CFLAGS	= $(COPTS)  $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

SYS_LIB	= ../lib/libsys.$(LIB_EXT)
LIBS	= $(SYS_LIB)

# ------------------------------------------------------------------------
# source list
#
OBJS	= \
	hlpbld.$(OBJ_EXT) \
	hlppass1.$(OBJ_EXT) \
	hlppass2.$(OBJ_EXT) \
	hlpsymbol.$(OBJ_EXT)

HDRS	= ${OBJS:.$(OBJ_EXT)=.h} hlpmgrif.h
CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

SRC_LIST	= makefile $(CSRC) $(HDRS) $(LANGUAGES:=.txt)

# ------------------------------------------------------------------------
# build targets
#
all : $(HLPBLD) $(HELP_FILES) check-helps $(TARGET)

$(HLPBLD): $(OBJS) $(LIBS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) $(OS_LIBS) -o $@

english.hlp english.h : english.txt
	$(TEXTSUB) -l english english.txt > english.tmp
	$(HLPBLD) english.tmp
	rm -f english.tmp

german.hlp german.h : german.txt
	$(TEXTSUB) -l german  german.txt  > german.tmp
	$(HLPBLD) german.tmp
	rm -f english.tmp

$(TARGET) : $(RESLANG).h
	cp $? $@

# ------------------------------------------------------------------------
# check all languages against the "default" for consistency
#
check-helps :
	@for i in $(LANGUAGES); \
	do \
		[ "$$i" = "$(RESLANG)" ] || \
		{ \
			$(HDRCOMP) $$i.h $(RESLANG).h || exit 1; \
		} \
	done

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER) . $(HELP_FILES)

# ------------------------------------------------------------------------
# targets to be installed
#
install : all
	@$(INSTALL) . $(HELP_FILES)

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
	rm -f *.$(OBJ_EXT) *.tmp $(HLPBLD) $(TARGET) $(OS_CLEAN)
	rm -f $(HELP_FILES) $(HELP_FILES:.hlp=.h)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
