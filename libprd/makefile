# ------------------------------------------------------------------------
#	program library makefile
#
#	Targets:
#		libprd			the program library
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libprd
TARGET	= $(LIB_TGT)/$(NAME).$(LIB_EXT)

# ------------------------------------------------------------------------
# cmd options
#
SYS_DIR	= ../libsys
OSD_DIR	= ../libos
CUR_DIR	= ../libecurses
UTS_DIR	= ../libutils
FTP_DIR	= ../libftp
ZIP_DIR	= ../libgzip
MGC_DIR	= ../libmagic
DIF_DIR	= ../libdiff
RES_DIR	= ../libres
LNG_DIR	= ../libres/$(RESLANG)
HLP_DIR	= ../libhelp
XVT_DIR = ../libxvt

CINCS	= \
	-I$(XVT_DIR) \
	-I$(HLP_DIR) \
	-I$(RES_DIR) \
	-I$(LNG_DIR) \
	-I$(DIF_DIR) \
	-I$(ZIP_DIR) \
	-I$(MGC_DIR) \
	-I$(FTP_DIR) \
	-I$(UTS_DIR) \
	-I$(CUR_DIR) \
	-I$(OSD_DIR) \
	-I$(SYS_DIR)

CFLAGS	= $(COPTS) $(OPTS) $(CDEFS) $(CINCS)
LFLAGS	= $(LOPTS) $(LDEFS) $(CINCS)

# ------------------------------------------------------------------------
# source files
#
HDRS	= \
	arch.h \
	config.h \
	consts.h \
	diff.h \
	errmsg.h \
	externs.h \
	gblvars.h \
	help.h \
	hxcb.h \
	menu.h \
	optenums.h \
	options.h \
	print.h \
	resource.h \
	structs.h \
	taginfo.h \
	vfcb.h \
	xgetstr.h \
	xglist.h \
	libprd.h

OBJS	=	\
	ablk.$(OBJ_EXT) \
	about.$(OBJ_EXT) \
	altcmd.$(OBJ_EXT) \
	arcdcmds.$(OBJ_EXT) \
	arcfcmds.$(OBJ_EXT) \
	archive.$(OBJ_EXT) \
	arcutils.$(OBJ_EXT) \
	args.$(OBJ_EXT) \
	attr.$(OBJ_EXT) \
	autoview.$(OBJ_EXT) \
	avail.$(OBJ_EXT) \
	avcmds.$(OBJ_EXT) \
	avcmds2.$(OBJ_EXT) \
	backup.$(OBJ_EXT) \
	bang.$(OBJ_EXT) \
	bar.$(OBJ_EXT) \
	batch.$(OBJ_EXT) \
	border.$(OBJ_EXT) \
	cfgdata.$(OBJ_EXT) \
	chkdir.$(OBJ_EXT) \
	chkfile.$(OBJ_EXT) \
	cmncmds.$(OBJ_EXT) \
	cmndcmds.$(OBJ_EXT) \
	cmnfcmds.$(OBJ_EXT) \
	cmp.$(OBJ_EXT) \
	comp.$(OBJ_EXT) \
	compact.$(OBJ_EXT) \
	compare.$(OBJ_EXT) \
	config.$(OBJ_EXT) \
	consts.$(OBJ_EXT) \
	copy.$(OBJ_EXT) \
	cpio.$(OBJ_EXT) \
	credits.$(OBJ_EXT) \
	crnl.$(OBJ_EXT) \
	date.$(OBJ_EXT) \
	dblk.$(OBJ_EXT) \
	default.$(OBJ_EXT) \
	dflttbl.$(OBJ_EXT) \
	delete.$(OBJ_EXT) \
	dest.$(OBJ_EXT) \
	diff.$(OBJ_EXT) \
	diffcmds.$(OBJ_EXT) \
	diffprt.$(OBJ_EXT) \
	dirrtns.$(OBJ_EXT) \
	dispcmds.$(OBJ_EXT) \
	dispdir.$(OBJ_EXT) \
	dispfile.$(OBJ_EXT) \
	edit.$(OBJ_EXT) \
	errmsg.$(OBJ_EXT) \
	extbar.$(OBJ_EXT) \
	extcpio.$(OBJ_EXT) \
	extract.$(OBJ_EXT) \
	exttar.$(OBJ_EXT) \
	extztar.$(OBJ_EXT) \
	falist.$(OBJ_EXT) \
	fblk.$(OBJ_EXT) \
	fileattr.$(OBJ_EXT) \
	filespec.$(OBJ_EXT) \
	ftpcopy.$(OBJ_EXT) \
	ftpdcmds.$(OBJ_EXT) \
	ftpdir.$(OBJ_EXT) \
	ftpfcmds.$(OBJ_EXT) \
	ftpfile.$(OBJ_EXT) \
	ftplog.$(OBJ_EXT) \
	ftpmisc.$(OBJ_EXT) \
	ftpxnode.$(OBJ_EXT) \
	ftpxutil.$(OBJ_EXT) \
	futils.$(OBJ_EXT) \
	getft.$(OBJ_EXT) \
	gblvars.$(OBJ_EXT) \
	getnode.$(OBJ_EXT) \
	getperms.$(OBJ_EXT) \
	global.$(OBJ_EXT) \
	goto.$(OBJ_EXT) \
	graft.$(OBJ_EXT) \
	help.$(OBJ_EXT) \
	hexcmds.$(OBJ_EXT) \
	hexedit.$(OBJ_EXT) \
	hide.$(OBJ_EXT) \
	info.$(OBJ_EXT) \
	intmain.$(OBJ_EXT) \
	loadenv.$(OBJ_EXT) \
	loadres.$(OBJ_EXT) \
	logbar.$(OBJ_EXT) \
	logcpio.$(OBJ_EXT) \
	logtar.$(OBJ_EXT) \
	logztar.$(OBJ_EXT) \
	match.$(OBJ_EXT) \
	menu.$(OBJ_EXT) \
	mdl.$(OBJ_EXT) \
	move.$(OBJ_EXT) \
	menupick.$(OBJ_EXT) \
	nblk.$(OBJ_EXT) \
	newnode.$(OBJ_EXT) \
	nodutils.$(OBJ_EXT) \
	open.$(OBJ_EXT) \
	pcmd.$(OBJ_EXT) \
	pcmdm.$(OBJ_EXT) \
	pgm.$(OBJ_EXT) \
	pgmhelp.$(OBJ_EXT) \
	pinput.$(OBJ_EXT) \
	print.$(OBJ_EXT) \
	prompt.$(OBJ_EXT) \
	prune.$(OBJ_EXT) \
	putnode.$(OBJ_EXT) \
	quit.$(OBJ_EXT) \
	regdcmds.$(OBJ_EXT) \
	regfcmds.$(OBJ_EXT) \
	rename.$(OBJ_EXT) \
	restore.$(OBJ_EXT) \
	search.$(OBJ_EXT) \
	setupdsp.$(OBJ_EXT) \
	showall.$(OBJ_EXT) \
	sort.$(OBJ_EXT) \
	split.$(OBJ_EXT) \
	stats.$(OBJ_EXT) \
	sym.$(OBJ_EXT) \
	tagfile.$(OBJ_EXT) \
	tar.$(OBJ_EXT) \
	term.$(OBJ_EXT) \
	traverse.$(OBJ_EXT) \
	treespec.$(OBJ_EXT) \
	usage.$(OBJ_EXT) \
	usrgrp.$(OBJ_EXT) \
	util.$(OBJ_EXT) \
	util2.$(OBJ_EXT) \
	view.$(OBJ_EXT) \
	viewcmd.$(OBJ_EXT) \
	viewdisp.$(OBJ_EXT) \
	what.$(OBJ_EXT) \
	xaddstr.$(OBJ_EXT) \
	xform.$(OBJ_EXT) \
	xgetch.$(OBJ_EXT) \
	xgetstr.$(OBJ_EXT) \
	xglist.$(OBJ_EXT) \
	xgmenu.$(OBJ_EXT) \
	xsystem.$(OBJ_EXT)

CSRC	= ${OBJS:.$(OBJ_EXT)=.c} mkicon.c

SRC_LIST	= makefile pgm-icon.txt $(HDRS) $(CSRC)

# ------------------------------------------------------------------------
# build targets
#
all : $(TARGET)

$(TARGET) : $(OBJS)
	$(MAKELIB) $@ $?

# ------------------------------------------------------------------------
# file dependencies
#
$(OBJS) : \
	gblvars.h \
	$(LNG_DIR)/cmds.h \
	$(LNG_DIR)/dflt.h \
	$(LNG_DIR)/errs.h \
	$(LNG_DIR)/msgs.h \
	$(LNG_DIR)/pgmi.h

term.$(OBJ_EXT) : pgm.xbm

pgm.xbm : pgm-icon.txt mkicon$(EXE_EXT)
	mkicon pgm-icon.txt

mkicon$(EXE_EXT) : mkicon.$(OBJ_EXT)
	$(CC) $(LDFLAGS) $? -o $@

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
	rm -f *.$(OBJ_EXT) mkicon$(EXE_EXT) pgm.xbm $(TARGET) $(OS_CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.$(OBJ_EXT) : %.c
	$(CC) -c $(CFLAGS) $<

%.i : %.c
	$(CC) -E $(CFLAGS) $< | sed -e '/^[ 	]*$$/d' > $@
