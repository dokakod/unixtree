# ------------------------------------------------------------------------
#
#	Description:
#		Script to deliver files to product directory
#
#		usage: deliver [options] dir files ...
#
#	The "dir" specified is relative to $DST_DIR.
#

# ------------------------------------------------------------------------
# usage msg
#
usage ()
{
	echo "usage: $PGM [options] dir files ..."
	echo "options:"
	echo "  -n name  deliver as \"name\""
	echo "  -i       install instead of deliver"
	echo "  -p       deliver to program directory"
	echo "  -u       deliver to utility directory"
	echo "  -w       deliver to windows directory"
	echo "  -d ddir  deliver to <ddir>  directory"
}

# ------------------------------------------------------------------------
# get options
#
PGM=`basename $0`
PERMS="ug+w,o-w"

if [ -c /dev/null ]
then
	DEV_NULL=/dev/null
else
	DEV_NULL=NUL
fi

NAME=
TGT_DIR="$DST_DIR"
ACTION=deliver

while [ "$1" != "" ]
do
	case "$1" in

	-n)
		NAME="$2"
		shift 2
		;;

	-p)
		TGT_DIR="$DST_DIR"
		shift
		;;

	-u)
		TGT_DIR="$UTL_DIR"
		shift
		;;

	-w)
		TGT_DIR="$WIN_DIR"
		shift
		;;

	-d)
		TGT_DIR="$2"
		shift 2
		;;

	-i)
		TGT_DIR="$INS_DIR"
		ACTION=install
		shift
		;;

	'-?' | -help | --help)
		usage
		exit 0
		;;

	--)
		shift
		break
		;;

	-*)
		echo "$PGM: unknown option $1" >&2
		usage >&2
		exit 1
		;;

	*)
		break
		;;
	esac
done

# ------------------------------------------------------------------------
# Make sure we have a directory to deliver to
#
if [ "$TGT_DIR" = "" ]
then
	echo "$PGM: no target directory set" >&2
	exit 1
fi

# ------------------------------------------------------------------------
# Make sure we have a sub-dir to deliver to
#
DIR="$1"
if [ "$DIR" = "" ]
then
	echo "$PGM: no sub-directory specified" >&2
	usage >&2
	exit 1
fi

shift

# ------------------------------------------------------------------------
# Make sure we have some args to process.
#
files="$@"

if [ "$files" = "" ]
then
	echo "$PGM: no files specified" >&2
	usage >&2
	exit 1
fi

# ------------------------------------------------------------------------
# Now make sure the delivery directory exists.
#
# We do this in two stages since some UNIXes barf if you do a
# "mkdir -p foo/."
#
if [ ! -d "$TGT_DIR" ]
then
	mkdir -p "$TGT_DIR" || exit 1
fi

if [ ! -d "$TGT_DIR/$DIR" ]
then
	mkdir -p "$TGT_DIR/$DIR" || exit 1
fi

# ------------------------------------------------------------------------
# Now copy the files over to the target area one by one.  We always remove
# the current copy that is there so we don't have permission problems and
# the like.
#
for src_file in $files
do
	# --------------------------------------------------------------------
	# get proper target name
	#
	SRC_BASE=`basename "$src_file"`
	if [ "$NAME" = "" ]
	then
		dst_file="$TGT_DIR/$DIR/$SRC_BASE"
	else
		dst_file="$TGT_DIR/$DIR/$NAME"
	fi

	# --------------------------------------------------------------------
	# check if copy is needed
	#
	if chknewer "$src_file" "$dst_file"
	then
		# ----------------------------------------------------------------
		# display msg
		#
		DST_BASE=`basename "$dst_file"`
		TGT_BASE=`basename "$TGT_DIR"`

		if [ "$DIR" = "." ]
		then
			DEST="$TGT_BASE/$DST_BASE"
		else
			DEST="$TGT_BASE/$DIR/$DST_BASE"
		fi

		if [ "$NAME" = "" ]
		then
			echo "--> $ACTION $DEST"
		else
			echo "--> $ACTION $DEST (from $SRC_BASE)"
		fi

		# ----------------------------------------------------------------
		# remove old file
		#
		rm -f "$dst_file" || exit 1

		# ----------------------------------------------------------------
		# copy new file
		#
		cp "$src_file" "$dst_file" || exit 1

		# ----------------------------------------------------------------
		# set permissions of new file
		#
		chmod $PERMS "$dst_file" || exit 1
	fi
done

exit 0
