# ------------------------------------------------------------------------
#	panel make file
#
#	Targets:
#		libpanel		panel library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libpanel
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
CINCS	= -I../libecurses
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	nc_panel.h \
	panel.h \
	panel.priv.h

OBJS	= \
	nc_panel.$(OBJ_EXT) \
	p_above.$(OBJ_EXT) \
	p_below.$(OBJ_EXT) \
	p_bottom.$(OBJ_EXT) \
	p_delete.$(OBJ_EXT) \
	p_hidden.$(OBJ_EXT) \
	p_hide.$(OBJ_EXT) \
	p_move.$(OBJ_EXT) \
	p_new.$(OBJ_EXT) \
	p_replace.$(OBJ_EXT) \
	p_show.$(OBJ_EXT) \
	p_top.$(OBJ_EXT) \
	p_update.$(OBJ_EXT) \
	p_user.$(OBJ_EXT) \
	p_win.$(OBJ_EXT) \
	panel.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile $(HDRS) $(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all :	$(TARGET)

$(TARGET) : $(OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# targets to be delivered
#
deliver : all
	@$(DELIVER) -w lib $(TARGET)
	@$(DELIVER) -w include panel.h

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
