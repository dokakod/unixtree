# ------------------------------------------------------------------------
#	curses make file
#
#	Targets:
#		libwin		curses library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libwin
TARGET_LIB	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

TARGET_HDR	= curses.h

TARGET	= $(TARGET_HDR) $(TARGET_LIB)

# ------------------------------------------------------------------------
# cmd options
#
TCP_DIR	= ../libtcap
TRM_DIR	= ../libterm
SYS_DIR	= ../libsys

CINCS	= -I$(TCP_DIR) -I$(TRM_DIR) -I$(SYS_DIR)
CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	ecurses.h \
	term.h \
	termcap.h \
	wblklets.h \
	wbox.h \
	wchtype.h \
	wcurses.h \
	wscreen.h \
	wtcinfo.h \
	wincommon.h

OBJS	= \
	wacsmap.$(OBJ_EXT) \
	waddch.$(OBJ_EXT) \
	waddstr.$(OBJ_EXT) \
	walarm.$(OBJ_EXT) \
	wattr.$(OBJ_EXT) \
	wbkgd.$(OBJ_EXT) \
	wblklets.$(OBJ_EXT) \
	wblkltr.$(OBJ_EXT) \
	wbox.$(OBJ_EXT) \
	wcenter.$(OBJ_EXT) \
	wclr2bot.$(OBJ_EXT) \
	wclr2eol.$(OBJ_EXT) \
	wcopywin.$(OBJ_EXT) \
	wdelch.$(OBJ_EXT) \
	wdelln.$(OBJ_EXT) \
	wdelwin.$(OBJ_EXT) \
	wdoupdate.$(OBJ_EXT) \
	wdupwin.$(OBJ_EXT) \
	wendwin.$(OBJ_EXT) \
	wgetch.$(OBJ_EXT) \
	wgetstr.$(OBJ_EXT) \
	winchns.$(OBJ_EXT) \
	winclock.$(OBJ_EXT) \
	windata.$(OBJ_EXT) \
	winitscr.$(OBJ_EXT) \
	winmouse.$(OBJ_EXT) \
	winnstr.$(OBJ_EXT) \
	winprt.$(OBJ_EXT) \
	winsch.$(OBJ_EXT) \
	winsdeln.$(OBJ_EXT) \
	winsln.$(OBJ_EXT) \
	winscreen.$(OBJ_EXT) \
	winsnap.$(OBJ_EXT) \
	wintcap.$(OBJ_EXT) \
	winutils.$(OBJ_EXT) \
	wmacros.$(OBJ_EXT) \
	wmove.$(OBJ_EXT) \
	wmvcur.$(OBJ_EXT) \
	wmvprintw.$(OBJ_EXT) \
	wmvscanw.$(OBJ_EXT) \
	wmvwin.$(OBJ_EXT) \
	wmvwprntw.$(OBJ_EXT) \
	wmvwscanw.$(OBJ_EXT) \
	wnewpad.$(OBJ_EXT) \
	wnewwin.$(OBJ_EXT) \
	wprefresh.$(OBJ_EXT) \
	wprintw.$(OBJ_EXT) \
	wprtscrn.$(OBJ_EXT) \
	wputwin.$(OBJ_EXT) \
	wredrawl.$(OBJ_EXT) \
	wrefresh.$(OBJ_EXT) \
	wripoff.$(OBJ_EXT) \
	wscanw.$(OBJ_EXT) \
	wscrdump.$(OBJ_EXT) \
	wscreen.$(OBJ_EXT) \
	wscrl.$(OBJ_EXT) \
	wscrlbar.$(OBJ_EXT) \
	wslk.$(OBJ_EXT) \
	wsubwin.$(OBJ_EXT) \
	wsync.$(OBJ_EXT) \
	wtcinfo.$(OBJ_EXT) \
	wterm.$(OBJ_EXT) \
	wterminal.$(OBJ_EXT) \
	wtouchln.$(OBJ_EXT) \
	wunctrl.$(OBJ_EXT) \
	wvwprintw.$(OBJ_EXT) \
	wvwscanw.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= makefile mkexts.sh $(HDRS) $(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

$(TARGET_LIB) : $(OBJS)
	$(MAKELIB) $@ $?

CURSES_SUBHDRS	= \
	../libterm/termkeys.h \
	../libterm/termattrs.h \
	wchtype.h \
	wbox.h

curses.h : ecurses.h $(CURSES_SUBHDRS)
	( \
		sed -n -e '1,/+++API+++/p' ecurses.h; \
		sed -n -e '/+++API+++/,/---API---/p' $(CURSES_SUBHDRS); \
		sed -n -e '/---API---/,$$p' ecurses.h; \
	) | sed -e '/[+-][+-][+-]API[+-][+-][+-]/d' > $@

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
