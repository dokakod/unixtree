# ------------------------------------------------------------------------
# list all files specified, prefixing them with a "relative" path
#

# ------------------------------------------------------------------------
# usage msg
#
usage ()
{
	echo "usage: $PGM [options] file ..."
	echo "options:"
	echo "  -d dir  Top level directory"
}

# ------------------------------------------------------------------------
# check arguments
#
PGM=$0

ARGS="`getopt "d:h:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-d)
		SRC_DIR="$2"
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
# Get current directory minus the base part.
#
BASE=`cd $SRC_DIR; pwd`
PREFIX=`pwd | sed -e "s,$BASE/*,,"`
[ "$PREFIX" = "" ] && PREFIX=.

# ------------------------------------------------------------------------
# Now process all files on the cmd line
#
for file
do
	# --------------------------------------------------------------------
	# output error msg if file doesn't exist
	#
	[ ! -f $file ] && echo "ERROR: cannot find file $PREFIX/$file" >&2

	# --------------------------------------------------------------------
	# Now output the filename
	#
	echo "$PREFIX/$file"
done | sort
