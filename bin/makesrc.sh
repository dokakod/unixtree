# ------------------------------------------------------------------------
#	Script to create a clone of a source tree
#
#	This script makes a clone of an original source tree by
#	creating a copy of the original tree with symlinks to all source files.
#
#	It creates a copy of all sub-directories in the original tree
#	and creates in each directory a symlink to the original
#	directory.  By default, this symlink is called ".src".
#	The contents of this entry depends on the "type" of entry desired.
#	It then creates a symlink in that directory for each file
#	in the original directory, pointing that symlink to ".src"/<file>.
#
# ------------------------------------------------------------------------
#
#	For example, assume the following source tree:
#
#		<yada-yada>/master
#		|	makefile
#		|
#		+---sub-dir-1
#		|	|	foo.c
#		|	|	foo.h
#		|	|
#		|	+---sub-sub-dir-1
#		|			hoser.c
#		|			hoser.h
#		|
#		+---sub-dir-2
#				bar.c
#				bar.h
#
#	Now we create the "clone" in "<yada-yada-yada>/clone":
#
#		$ mkdir <yada-yada-yada>/clone
#		$ cd <yada-yada-yada>/clone
#		$ make.src <yada-yada>/master
#
#	This will create the following tree:
#
#		<yada-yada-yada>/clone
#		|	.src -> <yada-yada>/master
#		|	makefile -> .src/makefile
#		|
#		+---sub-dir-1
#		|	|	.src -> ../.src/sub-dir-1
#		|	|	foo.c -> .src/foo.c
#		|	|	foo.h -> .src/foo.h
#		|	|
#		|	+---sub-sub-dir-1
#		|			.src -> ../.src/sub-sub-dir-1
#		|			hoser.c -> .src/hoser.c
#		|			hoser.h -> .src/hoser.h
#		|
#		+---sub-dir-2
#				.src -> ../.src/sub-dir-2
#				bar.c -> .src/bar.c
#				bar.h -> .src/bar.h
#
# ------------------------------------------------------------------------
#
#	The advantages of such a clone are:
#
#	1.	In a multiple-platform environment, you can have copies
#		of the tree for each platform (to build in) with practically
#		no overhead of extra disk-space for each tree.
#
#	2.	By updating a file in the "master" tree, all "clone" trees
#		automatically see the change with no extra work.
#
#	3.	It is very easy in a "clone" to distinguish "source" files
#		from "built" files, since all "source" files are symlinks
#		and all "built" files are real files.
#
#	4.	It gives the ability to have multiple copies of a tree with
#		no regard to what type of "source control" system you are using.
#		(If you are building on multiple platforms, they may not all
#		have the same source-control mechanism available.)
#
#	The "make.src" script can be run in an existing clone.  It will only
#	create new symlinks (and/or directories) for files which don't
#	already exist.
#
#	You can also run "make.src" within any sub-directory, providing
#	that sub-directory was already created via a top-level "make.src".
#	In this case, run it against the appropriate <master-dir>/sub-dir.
#
#	You can also exclude all sub-dirs of a particular name.  This
#	is useful for skipping sub-dirs such as "SCCS", "RCS", etc.
#
#	If "relative" symlink references are used, then you cannot run "make.src"
#	from a sub-dir in the tree.  You must always run it from the top-level.
#
# ------------------------------------------------------------------------
#
#	SYMLINK TYPES
#
#	By default, this script produces "incremental" entries, which are
#	nested symlink references.  There is a maximum number of nested
#	symlinks allowed in a reference (which varies for different platforms),
#	but is usually 16.
#
#	If your source tree is deeper than that many levels at any given point,
#	then you can use either "relative" or "absolute" references instead.
#	A "relative" reference will point all the way back to the top-level
#	directory entry.  An "absolute" reference will point directly to
#	the corresponding sub-directory in the master-directory.
#
#	For example, the ".src" entry for "sub-sub-dir-1" in the above example
#	would be:
#
#		incremental	->	../.src/sub-sub-dir-1
#		relative	->	../../.src/sub-dir-1/sub-sub-dir-1
#		absolute	->	<yada-yada>/master/sub-dir-1/sub-sub-dir-1
#
# ------------------------------------------------------------------------

# ------------------------------------------------------------------------
# usage msg
#
usage ()
{
	echo "usage: $PGM [options] [dir]"
	echo "options:"
	echo "  -v        Verbose mode"
	echo "  -i        Create \"incremental\" symlink references (default)"
	echo "  -r        Create \"relative\"    symlink references"
	echo "  -a        Create \"absolute\"    symlink references"
	echo "  -n name   Use \"name\" as intermediate name"
	echo "            (default is \"$SRC_DEFAULT\")"
	echo "  -e dir    Exclude any sub-dir named \"dir\" (and all its sub-dirs)"
	echo "  -f files  Specify a list of files to link"
	echo "dir         Name of master directory to clone"
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
# create a directory
#
make_dir ()
{
	dir="$1"

	[ -d "$dir" ] ||
	{
		[ "$VERBOSE" = "yes" ] && echo "  Making directory $dir"
		mkdir -p "$dir"
	}
}

# ------------------------------------------------------------------------
# get name of link-point
#
get_link_name ()
{
	dir="$1"

	if [ "$dir" = "." ]
	then
		subdir="$MASTER_DIR"
	else
		case $SYM_TYPE in
		i)
			BASE=`basename "$dir"`
			subdir="../$SRC/$BASE"
			;;

		r)
			PREFIX=`echo "dir" | sed -e 's,[^/][^/]*,..,g'`
			subdir="$PREFIX/$SRC/$dir"
			;;

		a)
			subdir="$MASTER_DIR/$dir"
			;;
		esac
	fi

	echo "$subdir"
}

# ------------------------------------------------------------------------
# create link-point if needed
#
create_src ()
{
	src="$1"
	subdir="$2"

	if [ ! -L "$src" ]
	then
		[ "$VERBOSE" = "yes" ] && echo "  Adding $src -> \"$subdir\""
		ln -s "$subdir" "$src"
	fi
}

# ------------------------------------------------------------------------
# process all sub-directories
#
do_sub_dirs ()
{
	(cd "$MASTER_DIR"; find . -type d -print | sort) |
	while read dir
	do
		# ----------------------------------------------------------------
		# remove leading "./" if not top-level dir
		#
		[ "$dir" != "." ] && dir=`echo "$dir" | cut -c3-`

		# ----------------------------------------------------------------
		# check if we should exclude this dir
		#
		if [ "$EXCLUDE" != "" ]
		then
			SKIP=no
			for i in `echo "$dir" | sed -e 's,/, ,'`
			do
				[ "$i" = "$EXCLUDE" ] && { SKIP=yes; break; }
			done
			[ "$SKIP" = "yes" ] && continue
		fi

		# ----------------------------------------------------------------
		# display sub-dir name
		#
		echo "$dir"

		# ----------------------------------------------------------------
		# create sub-directory if it doesn't exist
		#
		make_dir "$dir" || continue

		# ----------------------------------------------------------------
		# get name to point $SRC to
		#
		SUBDIR=`get_link_name "$dir"`

		# ----------------------------------------------------------------
		# process sub-directory
		#
		(
			cd "$dir"

			# ------------------------------------------------------------
			# create $SRC if it doesn't exist
			#
			create_src "$SRC" "$SUBDIR"

			# ------------------------------------------------------------
			# now process all files in sub-directory
			#
			link_src
		)
	done
}

# ------------------------------------------------------------------------
# process file list
#
do_files ()
{
	cat "$FILES" |
	while read line
	do
		dir=` dirname  $line`
		file=`basename $line`

		make_dir "$dir" || continue
		sub_dir=`get_link_name "$dir"`
		(
			cd "$dir"
			create_src "$SRC" "$sub_dir"

			if [ -f "$SRC"/"$file" -a ! -f "$file" ]
			then
				[ "$VERBOSE" = "yes" ] && echo "  Adding $file"
				ln -s "$SRC"/"$file" "$file"
			fi
		)
	done
}

# ------------------------------------------------------------------------
# set all defaults
#
PGM=$0					export PGM

SRC_DEFAULT=".src"		export SRC_DEFAULT
SRC="$SRC_DEFAULT"		export SRC
VERBOSE=no				export VERBOSE
EXCLUDE=				export EXCLUDE
SYM_TYPE=i				export SYM_TYPE
FILES=					export FILES

# ------------------------------------------------------------------------
# process all options
#
ARGS="`getopt "viran:e:f:h:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

while [ "$1" != "" ]
do
	case "$1" in

	-v)
		VERBOSE=yes
		shift
		;;

	-i)
		SYM_TYPE=i
		shift
		;;

	-a)
		SYM_TYPE=a
		shift
		;;

	-r)
		SYM_TYPE=r
		shift
		;;

	-n)
		SRC="$2"
		shift 2
		;;

	-e)
		EXCLUDE="$2"
		shift 2
		;;

	-f)
		FILES="$2"
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
# get master directory name
#
if [ $# -gt 1 ]
then
	echo "$PGM: too many args" >&2
	usage >&2
	exit 1

elif [ $# -lt 1 ]
then
	if [ ! -L "$SRC" ]
	then
		echo "$PGM: no directory specified and no \"$SRC\" exists" >&2
		usage >&2
		exit 1
	fi

	MASTER_DIR=`cd "$SRC"; pwd`

else
	MASTER_DIR="$1"
fi

# ------------------------------------------------------------------------
# check master directory
#
if [ ! -d "$MASTER_DIR" ]
then
	if [ -f "$MASTER_DIR" ]
	then
		echo "$PGM: \"$MASTER_DIR\" is not a directory" >&2
	else
		echo "$PGM: \"$MASTER_DIR\" does not exist" >&2
	fi
	exit 1
fi

MASTER_DIR="`cd $MASTER_DIR; pwd`"

# ------------------------------------------------------------------------
# now do it
#
if [ "$FILES" = "" ]
then
	do_sub_dirs
else
	do_files
fi
