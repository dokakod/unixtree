# ------------------------------------------------------------------------
#	script to compare 2 header files
#
#	This script removes all lines except "define" lines
#	and all comments, then compares the two files
#

# ------------------------------------------------------------------------
# usage
#
usage ()
{
	echo "usage: $PGM hdr-1 hdr-2"
}

# ------------------------------------------------------------------------
# strip header file into a temp file
#
strip_file()
{
	hdr=$1
	tmp=$2

	sed -n -e 's,[ 	]*/\*.*,,' -e '/^#[ 	]*define/p' $hdr >$tmp
}

# ------------------------------------------------------------------------
# process options
#
PGM=$0

ARGS="`getopt "h:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

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
# get files to compare
#
if [ $# -ne 2 ]
then
	usage >&2
	exit 1
fi

HDR_1=$1
HDR_2=$2

# ------------------------------------------------------------------------
# check if second arg is a directory
#
[ -d $HDR_2 ] && HDR_2=$HDR_2/`basename $HDR_1`

# ------------------------------------------------------------------------
# get temp files to create
#
TMP_1=tmp$$.1
TMP_2=tmp$$.2

# ------------------------------------------------------------------------
# create the temp files
#
strip_file $HDR_1 $TMP_1
strip_file $HDR_2 $TMP_2

# ------------------------------------------------------------------------
# now compare them
#
if [ -c /dev/null ]
then
	DEV_NULL=/dev/null
else
	DEV_NULL=NUL
fi

diff $TMP_1 $TMP_2 >$DEV_NULL 2>&1
RESULT=$?

# ------------------------------------------------------------------------
# delete the temp files
#
rm -f $TMP_1 $TMP_2

# ------------------------------------------------------------------------
# output message if not same
#
if [ $RESULT -ne 0 ]
then
	echo "Header files $HDR_1 & $HDR_2 do not match." >&2
fi

exit $RESULT
