# ------------------------------------------------------------------------
#	script to run "nm" and "clean up" the output
#
#	The original lines look like:
#
#	Unix:
#		nnnnnnnnn <type> file.o [$xxxxxxx.]name
#
#	Windows:
#		file.obj: 0:nnnnnnnn <type> name
#
#	We clean it up to look like:
#
#		file	type	name
#
#	where type is:
#
#		B	global BSS  uninitialized data symbol
#		b	local  BSS  uninitialized data symbol
#		D	global data   initialized data symbol
#		d	local  data   initialized data symbol
#		T	global procedure definition
#		t	local  procedure definition
#		U	external reference (data or procedure)
#
#	Note that this script would be a LOT more complicated (if not
#	impossible) if C++ objects were to be processed (and unmangled).
#

# ------------------------------------------------------------------------
# usage
#
usage ()
{
	echo "usage: $PGM [options] object-files ..."
	echo "options:"
	echo "  -s     Ignore static entries"
}

# ------------------------------------------------------------------------
# Windows version
#
nm_win ()
{
	[ "$STATICS" = "no" ] && STAT_ARG=-g

	nm.exe -A $STAT_ARG $* |
	sed \
		-e '/ \$/d' \
		-e '/ [AaFlNnSs] /d' \
		-e 's/ _/	/' \
		-e 's/_imp__//' \
		-e '/	_/d' \
		-e 's/:.* /	/' \
		-e 's/obj	/c	/' \
		-e 's/[A-Z]*@.*//'
}

# ------------------------------------------------------------------------
# Unix version
#
nm_unix ()
{
	[ "$STATICS" = "no" ] && STAT_ARG=-g

	nm -rnp $STAT_ARG $* |
	sed \
		-e '/^$/d' \
		-e '/^[^0]/d' \
		-e 's/[^ ]* //' \
		-e 's/[ :]/ /g' \
		-e '/^[^BDTtU]/d' \
		-e 's/o /c /' \
		-e '/ \./d' \
		-e '/\$/s/^D/d/' \
		-e '/\$/s/^B/b/' \
		-e 's/\$.*\.//' \
		-e 's/ /	/g' \
		-e '/	_/d' \
		-e 's/^\(.*\)	\(.*\)	\(.*\)/\2	\1	\3/'
}

# ------------------------------------------------------------------------
# check arguments
#
PGM=$0

STATICS=yes	export STATICS

ARGS="`getopt "sh:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-s)
		STATICS=no
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

ARGS="$*"

# ------------------------------------------------------------------------
# now do it
#
if [ -c /dev/null ]
then
	nm_unix $ARGS
else
	nm_win  $ARGS
fi | sort
