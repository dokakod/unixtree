# ------------------------------------------------------------------------
#	libftp makefile
#
#	Targets:
#		libftp			the ftp library
#

# ------------------------------------------------------------------------
# target name
#
NAME	= libftp
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CUR_DIR	= ../libecurses
UTS_DIR	= ../libutils
OSD_DIR	= ../libos
SYS_DIR	= ../libsys

CINCS	= -I$(CUR_DIR) -I$(UTS_DIR) -I$(OSD_DIR) -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS) $(NET_OPTS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS) $(NET_OPTS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	libftp.h \
	ftpcommon.h \
	ftpnode.h \
	ftpstat.h \
	ftpsvc.h

OBJS	= \
	ftp.$(OBJ_EXT) \
	ftpcmd.$(OBJ_EXT) \
	ftpcmds.$(OBJ_EXT) \
	ftpcmds2.$(OBJ_EXT) \
	ftplogin.$(OBJ_EXT) \
	ftpnet.$(OBJ_EXT) \
	ftpnode.$(OBJ_EXT) \
	ftpparsd.$(OBJ_EXT) \
	ftpparse.$(OBJ_EXT) \
	ftpparsu.$(OBJ_EXT) \
	ftpparsv.$(OBJ_EXT) \
	ftputils.$(OBJ_EXT)

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
