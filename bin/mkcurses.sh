# ------------------------------------------------------------------------
# This script will create a directory which will contain
# all "ecurses" sources & a makefile to create it.
#
# Usage:	mkcurses.sh [options] [dir]
#
# If <dir> is specified, it will be created if needed and all files
# are placed there.  Otherwise, it will be created in the current
# directory.
#
# The purpose of this script is to create a copy of all "ecurses" sources
# into a single directory which can then be used in another source tree.
#

# ------------------------------------------------------------------------
# usage
#
usage ()
{
	echo "usage: $PGM [options] [directory]"
	echo "options:"
	echo "  -v       verbose mode"
	echo "  -w       make files writable"
	echo "  -c       don't copy source files"
	echo "  -m file  specify makefile name"
}

# ------------------------------------------------------------------------
# output the template files
#
output_template1 ()
{
	cat <<EOF
# ------------------------------------------------------------------------
#	ecurses make file
#
#	Targets:
#		libcurses.a			extended curses library
#		curses.h			extended curses header
#

# ------------------------------------------------------------------------
# target names
#
NAME	= libcurses

TARGET_LIB	= \$(NAME).\$(LIB_EXT)
TARGET_HDR	= curses.h

TARGET	= \$(TARGET_HDR) \$(TARGET_LIB)

# ------------------------------------------------------------------------
# cmd options
#
CFLAGS	= \$(COPTS) \$(OPTS) \$(CDEFS) \$(CINCS) \$(SYS_OPTS) \$(TERM_OPTS)
LFLAGS	= \$(LOPTS) \$(LDEFS) \$(CINCS) \$(SYS_OPTS) \$(TERM_OPTS)
ARFLAGS	= rc

# ------------------------------------------------------------------------
# platform options
#
OBJ_EXT		= o
LIB_EXT		= a
OS_CLEAN	= core core.*

# ------------------------------------------------------------------------
# source files
#
EOF
}

output_template2 ()
{
	cat <<EOF
OBJS	= \${CSRC:.c=.\$(OBJ_EXT)}

# ------------------------------------------------------------------------
# list of all source files
#
SRC_LIST	= $MAKEFILE \$(HDRS) \$(CSRC)

# ------------------------------------------------------------------------
# list of files to be cleaned
#
CLEAN	= *.\$(OBJ_EXT) \$(TARGET) \$(OS_CLEAN)

# ------------------------------------------------------------------------
# build targets
#
all :	\$(TARGET)

\$(TARGET_LIB) : \$(OBJS)
	\$(AR) \$(ARFLAGS) \$@ \$?

CURSES_SUBHDRS	= \\
	termkeys.h \\
	termattrs.h \\
	wchtype.h \\
	wbox.h

\$(TARGET_HDR) : ecurses.h \$(CURSES_SUBHDRS)
	( \\
		sed -n -e '1,/+++API+++/p' ecurses.h; \\
		sed -n -e '/+++API+++/,/---API---/p' \$(CURSES_SUBHDRS); \\
		sed -n -e '/---API---/,\$\$p' ecurses.h; \\
	) | sed -e '/[+-][+-][+-]API[+-][+-][+-]/d' > \$@

# ------------------------------------------------------------------------
# lint all source files
#
lint : \$(CSRC)
	\$(LINT) \$(LFLAGS) \$?

# ------------------------------------------------------------------------
# list all source files
#
srclist :
	@ls \$(SRC_LIST)

# ------------------------------------------------------------------------
# clean all "built" files
#
clean :
	rm -f \$(CLEAN)

# ------------------------------------------------------------------------
# build rules
#
%.\$(OBJ_EXT) : %.c
	\$(CC) -c \$(CFLAGS) \$<

%.i : %.c
	\$(CC) -E \$(CFLAGS) \$< | sed -e '/^[ 	]*\$\$/d' > \$@
EOF
}

# ------------------------------------------------------------------------
# process include directories
#
process_includes ()
{
	# --------------------------------------------------------------------
	# first do the file lists
	#
	for LIB in $LIBS
	do
		# ----------------------------------------------------------------
		# get library name & directory
		#
		eval `echo "$LIB" | sed -e 's/^\(.*\):\(.*\)/NAME="\1" DIR="\2"/'`

		[ "$VERBOSE" = "yes" ] && echo "listing $DIR files" >&2

		# ----------------------------------------------------------------
		# process files in directory
		#
		(
			cd $SRC_DIR/$DIR

			# ------------------------------------------------------------
			# get list of *.h & *.c files
			#
			FILES=`make srclist | sed -n -e 's,.*/,,' -e '/\.[ch]$/p'`
			HSRC=`ls $FILES | grep '\.h$'`
			CSRC=`ls $FILES | grep '\.c$'`

			# ------------------------------------------------------------
			# output *.h list
			#
			echo "${NAME}_HDRS	= \\"
			for F in $HSRC
			do
				echo "	$F \\\\"
			done
			echo "	\$(EMPTY)"
			echo ""

			# ------------------------------------------------------------
			# output *.c list
			#
			echo "${NAME}_CSRC	= \\"
			for F in $CSRC
			do
				echo "	$F \\"
			done
			echo "	\$(EMPTY)"
			echo ""

			# ------------------------------------------------------------
			# now copy the files over
			#
			if [ "$COPYSRC" = "yes" ]
			then
				[ "$VERBOSE" = "yes" ] && echo "copying $DIR files" >&2
				cp $FILES $TGT_DIR
			fi
		)
	done

	[ "$VERBOSE" = "yes" ] && echo "listing build files" >&2

	# --------------------------------------------------------------------
	# now the top-level header list
	#
	echo "HDRS	= \\"
	for LIB in $LIBS
	do
		# ----------------------------------------------------------------
		# get library name & directory
		#
		eval `echo "$LIB" | sed -e 's/^\(.*\):\(.*\)/NAME="\1" DIR="\2"/'`
		echo "	\$(${NAME}_HDRS) \\"
	done
	echo "	\$(EMPTY)"
	echo ""

	# --------------------------------------------------------------------
	# now the top-level source list
	#
	echo "CSRC	= \\"
	for LIB in $LIBS
	do
		# ----------------------------------------------------------------
		# get library name & directory
		#
		eval `echo "$LIB" | sed -e 's/^\(.*\):\(.*\)/NAME="\1" DIR="\2"/'`
		echo "	\$(${NAME}_CSRC) \\"
	done
	echo "	\$(EMPTY)"
	echo ""
}

# ------------------------------------------------------------------------
# main()
#
PGM=`basename $0`

VERBOSE=no
WRITABLE=no
COPYSRC=yes
MAKEFILE=makefile

# ------------------------------------------------------------------------
# process any options
#
ARGS=`getopt "vcwh:m:?" $*` || { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-v)
		VERBOSE=yes
		shift
		;;

	-c)
		COPYSRC=no
		shift
		;;

	-w)
		WRITABLE=yes
		shift
		;;

	-m)
		MAKEFILE="$2"
		shift 2
		;;

	'-?' | -h)
		usage
		exit 0
		;;

	--)
		shift
		break
		;;

	-*)
		echo "$PGM: invalid option \"$1\"" >&2
		usage >&2
		exit 1
		;;

	*)
		break
		;;

	esac
done

# ------------------------------------------------------------------------
# check if build environment is setup
#
[ "$PLATFORM" != "" ] ||
{
	echo "No build environment setup." >&2
	echo "Run \". build <platform>\" first." >&2
	exit 1
}

# ------------------------------------------------------------------------
# check if a directory was specified & create it if needed
#
if [ "$1" != "" ]
then
	TGT_DIR="$1"
	if [ ! -d $TGT_DIR ]
	then
		[ "$VERBOSE" = "yes" ] && echo "creating directory \"$TGT_DIR\"" >&2
		mkdir -p $TGT_DIR || { echo "cannot create directory" >&2; exit 1; }
	else
		[ -w $TGT_DIR ] || { echo "\"$TGT_DIR\" not writable" >&2; exit 1; }
	fi
	cd $TGT_DIR
else
	TGT_DIR="."
	[ "$VERBOSE" = "yes" ] && echo "using current directory" >&2
	[ -w $TGT_DIR ] || { echo "current directory not writable" >&2; exit 1; }
fi

TGT_DIR=`pwd`

# ------------------------------------------------------------------------
# now do the actual work
#
# We create the makefile, and in the processing of the include
# lines in the makefile template, the source files are copied over.
#
LIBS=" \
	SYS:libsys \
	TRM:libterm \
	TCP:libtcap \
	WIN:libwin \
	"

[ "$VERBOSE" = "yes" ] && echo "creating $MAKEFILE" >&2

{
	output_template1
	process_includes
	output_template2
} > $TGT_DIR/$MAKEFILE

# ------------------------------------------------------------------------
# set files writable if requested
#
if [ "$WRITABLE" = "yes" ]
then
	if [ "$COPYSRC" = "yes" ]
	then
		[ "$VERBOSE" = "yes" ] && echo "setting all files writable" >&2
		chmod ug+w $TGT_DIR/*.[ch]
	fi
else
	chmod -w $TGT_DIR/$MAKEFILE
fi
