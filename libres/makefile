# ------------------------------------------------------------------------
#	resource-file make file
#
#	This makefile creates the following:
#
#		libres				The library for reading/writing resource files.
#
#		resmkch				A program used in the language sub-directories
#							to create the *.c & *.h files for each table.
#
#		restool				A stand-alone utility to process a resource file.
#							This program is not delivered.
#
#	For each language, the following is created:
#
#		libres-<language>	A library containing the compiled resource-file
#							for that language.
#
#		<language>.res		A resource file for that language.
#
#	The program can load any resource-file provided, but it is linked
#	with the resource-file for the "default" language.  This way, there
#	is always a backup resource table even if no resource files are present.
#
#	After creating resource files for each language defined, the headers
#	for each language are checked against the headers for the "default"
#	language ($RESLANG) to insure that all resource files are consistent.
#

# ------------------------------------------------------------------------
# language sub-directories
#
LANGUAGES	= \
	english \
	german

# ------------------------------------------------------------------------
# targets
#
TARGET		= $(LIB_TGT)/libres.$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR		= ../libsys

OSD_DIR		= ../libos
OSD_LIB		= ../lib/libos.$(LIB_EXT)

CUR_DIR		= ../libecurses
CUR_LIB		= ../lib/libcurses.$(LIB_EXT)

RES_LIB		= ../lib/libres.$(LIB_EXT)

CINCS		= -I$(CUR_DIR) -I$(OSD_DIR) -I$(SYS_DIR)
LINK_LIBS	= $(CUR_LIB) $(OSD_LIB)

CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)
LDFLAGS	= $(LDOPTS) $(OPTS) $(LDDEFS)

# ------------------------------------------------------------------------
# source files
#
HDRS		= \
	libres.h \
	res.h \
	restool.h

OBJS		= \
	reslib-f.$(OBJ_EXT) \
	reslib-r.$(OBJ_EXT) \
	reslib-t.$(OBJ_EXT) \
	reslib-w.$(OBJ_EXT)

UTIL_OBJS	= \
	rest-comp.$(OBJ_EXT) \
	rest-diff.$(OBJ_EXT) \
	rest-hdrs.$(OBJ_EXT) \
	rest-extr.$(OBJ_EXT) \
	rest-prnt.$(OBJ_EXT) \
	rest-main.$(OBJ_EXT)

CSRC		= \
	${OBJS:.$(OBJ_EXT)=.c} \
	${UTIL_OBJS:.$(OBJ_EXT)=.c}

SRC_LIST	= makefile resmkch.c $(HDRS) $(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all : \
	$(TARGET) \
	restool$(EXE_EXT) \
	resmkch$(EXE_EXT) \
	resource-files \
	check-resources

$(TARGET) : $(OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# resource-file tool
#
restool$(EXE_EXT) : $(UTIL_OBJS) $(RES_LIB) $(LINK_LIBS)
	$(CC) $(LDFLAGS) $(UTIL_OBJS) $(RES_LIB) $(LINK_LIBS) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# tool to create "foo.c" & "foo.h" from "foo.txt"
# (used in language sub-dirs)
#
resmkch$(EXE_EXT) : resmkch.$(OBJ_EXT) $(RES_LIB)
	$(CC) $(LDFLAGS) resmkch.$(OBJ_EXT) $(RES_LIB) $(OS_LIBS) -o $@

# ------------------------------------------------------------------------
# build all languages defined
#
resource-files :
	@for dir in $(LANGUAGES); \
	do \
		echo "=== building libres/$$dir"; \
		( cd $$dir; $(MAKE) all ) || exit 1; \
	done

# ------------------------------------------------------------------------
# check all languages against the "default" for consistency
#
check-resources :
	@for dir in $(LANGUAGES); \
	do \
		[ "$$dir" = "$(RESLANG)" ] || \
		{ \
			echo "=== checking libres/$$dir"; \
			for hdr in `cd $$dir; ls *.h`; \
			do \
				$(HDRCOMP) $$dir/$$hdr $(RESLANG)/$$hdr || exit 1; \
			done; \
		} \
	done

# ------------------------------------------------------------------------
# targets to be delivered
#
# We only deliver restool & resource files if $DELIVER_RES is set
# or there were two or more languages built.
#
deliver : all
	@[ "$(DELIVER_RES)" = "" -a `ls -d $(LANGUAGES) | wc -l` -eq 1 ] || \
	{ \
		$(DELIVER) . restool$(EXE_EXT); \
		for dir in $(LANGUAGES); \
		do \
			( cd $$dir; $(MAKE) $@ ) \
		done; \
	}

# ------------------------------------------------------------------------
# targets to be installed
#
# We only deliver restool & resource files if $DELIVER_RES is set
# or there were two or more languages built.
#
install : all
	@[ "$(DELIVER_RES)" = "" -a `ls -d $(LANGUAGES) | wc -l` -eq 1 ] || \
	{ \
		$(INSTALL) . restool$(EXE_EXT); \
		for dir in $(LANGUAGES); \
		do \
			( cd $$dir; $(MAKE) $@ ) \
		done; \
	}

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
	@for dir in $(LANGUAGES); \
	do \
		( cd $$dir; $(MAKE) $@ ) \
	done

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	-rm -f *.$(OBJ_EXT) $(RES_LIB) $(OS_CLEAN)
	-rm -f restool$(EXE_EXT) resmkch$(EXE_EXT)
	@for dir in $(LANGUAGES); \
	do \
		echo "=== cleaning libres/$$dir"; \
		( cd $$dir; $(MAKE) $@ ) \
	done

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
