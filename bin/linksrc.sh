# ------------------------------------------------------------------------
#	Create links for all files in a directory
#
#	This is a companion script to "make.src", which just creates
#	a copy of all missing files in a given directory.  It is assumed
#	that the directory was already created via "make.src".
#
#	See "make.src" for details.
#
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# usage msg
#
usage ()
{
	echo "usage: $PGM [options]"
	echo "options:"
	echo "  -v       Verbose mode"
	echo "  -n name  Use \"name\" as intermediate name"
	echo "           (default is \"$SRC_DEFAULT\")"
}

# ------------------------------------------------------------------------
# link all files in a directory
#
link_src ()
{
	(cd "$SRC"; ls -a) |
	while read file
	do
		# ----------------------------------------------------------------
		# skip . & ..
		#
		[ "$file" = "." -o "$file" = ".." ] && continue

		# ----------------------------------------------------------------
		# Create symlink to master file if name is a file and it
		# does not already exist.
		#
		if [ -f "$SRC"/"$file" -a ! -f "$file" ]
		then
			[ "$VERBOSE" = "yes" ] && echo "  Adding $file"
			ln -s "$SRC"/"$file" "$file"
		fi
	done
}

# ------------------------------------------------------------------------
# check arguments
#
PGM=$0

VERBOSE=no
SRC_DEFAULT=".src"
SRC="$SRC_DEFAULT"

ARGS="`getopt "vn:h:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-v)
		VERBOSE=yes
		shift
		;;

	-n)
		SRC="$2"
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
# verify that $SRC exists
#
if [ ! -L "$SRC" ]
then
	echo "$PGM: no \"$SRC\" found." >&2
	exit 1
fi

# ------------------------------------------------------------------------
# now process directory
#
link_src
