# ------------------------------------------------------------------------
# Create a xref-list of all object files in a source tree
#

# ------------------------------------------------------------------------
# usage
#
usage ()
{
	echo "usage: $PGM [options]"
	echo "options:"
	echo "  -s     Ignore static data"
	echo "  -v     Verbose mode"
}

# ------------------------------------------------------------------------
# check arguments
#
PGM=$0

if [ -c /dev/null ]
then
	DEV_NULL=/dev/null
else
	DEV_NULL=NUL
fi

STATICS=
VERBOSE=no

ARGS="`getopt "svh:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-s)
		STATICS="-s"
		shift
		;;

	-v)
		VERBOSE=yes
		shift
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
# now do it
#
find . -type d -print | sort | sed -e '/../s/..//' |
while read dir
do
	objs=`ls $dir/*.$OBJ_EXT 2>$DEV_NULL`
	if [ "$objs" != "" ]
	then
		[ "$VERBOSE" = "yes" ] && echo "=== xref $dir ===" >&2
		xrfobjs.sh $STATICS $objs
	fi
done
[ "$VERBOSE" = "yes" ] && echo "=== done ===" >&2
