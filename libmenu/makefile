# ------------------------------------------------------------------------
#	menu make file
#
#	Targets:
#		libmenu		menu library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libmenu
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
	eti.h \
	menu.h \
	menu.priv.h \
	mf_common.h

OBJS	= \
	m_attribs.$(OBJ_EXT) \
	m_cursor.$(OBJ_EXT) \
	m_driver.$(OBJ_EXT) \
	m_format.$(OBJ_EXT) \
	m_global.$(OBJ_EXT) \
	m_hook.$(OBJ_EXT) \
	m_item_cur.$(OBJ_EXT) \
	m_item_nam.$(OBJ_EXT) \
	m_item_new.$(OBJ_EXT) \
	m_item_opt.$(OBJ_EXT) \
	m_item_top.$(OBJ_EXT) \
	m_item_use.$(OBJ_EXT) \
	m_item_val.$(OBJ_EXT) \
	m_item_vis.$(OBJ_EXT) \
	m_items.$(OBJ_EXT) \
	m_new.$(OBJ_EXT) \
	m_opts.$(OBJ_EXT) \
	m_pad.$(OBJ_EXT) \
	m_pattern.$(OBJ_EXT) \
	m_post.$(OBJ_EXT) \
	m_req_name.$(OBJ_EXT) \
	m_scale.$(OBJ_EXT) \
	m_spacing.$(OBJ_EXT) \
	m_sub.$(OBJ_EXT) \
	m_userptr.$(OBJ_EXT) \
	m_win.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

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
	@$(DELIVER) -w include eti.h menu.h

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
