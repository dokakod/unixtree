# ------------------------------------------------------------------------
#	forms make file
#
#	Targets:
#		libform		forms library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libform
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
	form.h \
	form.priv.h \
	mf_common.h

OBJS	= \
	fld_arg.$(OBJ_EXT) \
	fld_attr.$(OBJ_EXT) \
	fld_current.$(OBJ_EXT) \
	fld_def.$(OBJ_EXT) \
	fld_dup.$(OBJ_EXT) \
	fld_ftchoice.$(OBJ_EXT) \
	fld_ftlink.$(OBJ_EXT) \
	fld_info.$(OBJ_EXT) \
	fld_just.$(OBJ_EXT) \
	fld_link.$(OBJ_EXT) \
	fld_max.$(OBJ_EXT) \
	fld_move.$(OBJ_EXT) \
	fld_newftyp.$(OBJ_EXT) \
	fld_opts.$(OBJ_EXT) \
	fld_pad.$(OBJ_EXT) \
	fld_page.$(OBJ_EXT) \
	fld_stat.$(OBJ_EXT) \
	fld_type.$(OBJ_EXT) \
	fld_user.$(OBJ_EXT) \
	frm_cursor.$(OBJ_EXT) \
	frm_data.$(OBJ_EXT) \
	frm_def.$(OBJ_EXT) \
	frm_driver.$(OBJ_EXT) \
	frm_hook.$(OBJ_EXT) \
	frm_opts.$(OBJ_EXT) \
	frm_page.$(OBJ_EXT) \
	frm_post.$(OBJ_EXT) \
	frm_req_name.$(OBJ_EXT) \
	frm_scale.$(OBJ_EXT) \
	frm_sub.$(OBJ_EXT) \
	frm_user.$(OBJ_EXT) \
	frm_win.$(OBJ_EXT) \
	fty_alnum.$(OBJ_EXT) \
	fty_alpha.$(OBJ_EXT) \
	fty_enum.$(OBJ_EXT) \
	fty_int.$(OBJ_EXT) \
	fty_ipv4.$(OBJ_EXT) \
	fty_num.$(OBJ_EXT) \
	fty_regex.$(OBJ_EXT)

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
	@$(DELIVER) -w lib $(TARGET)
	@$(DELIVER) -w include eti.h form.h

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
