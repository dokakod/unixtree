# ------------------------------------------------------------------------
#	CM script using perforce
#

# ------------------------------------------------------------------------
# It seems that the MKS version of sed doesn't honor the
#
#	y/A-Z/a-z/
#
# notation, so we have to do this the hard way with literal chars.
#
ATOZ=ABCDEFGHIJKLMNOPQRSTUVWXYZ
atoz=abcdefghijklmnopqrstuvwxyz

# ------------------------------------------------------------------------
# delete temp files
#
delete_temp_files ()
{
	rm -f $TMPFILE $CMTFILE
}

# ------------------------------------------------------------------------
# check cmd
#
check_cmd ()
{
	$DEBUG

	case "$CMD" in

	help)
		PROCEDURE=help
		NEED_FILE=no
		;;

	info)
		PROCEDURE=info
		NEED_FILE=no
		;;

	list)
		PROCEDURE=list
		NEED_FILE=no
		;;

	refresh)
		PROCEDURE=refresh
		NEED_FILE=no
		;;

	rebuild)
		PROCEDURE=rebuild
		NEED_FILE=no
		;;

	sync)
		PROCEDURE=sync
		NEED_FILE=no
		;;

	tell)
		PROCEDURE=tell
		NEED_FILE=no
		;;

	create)
		PROCEDURE=create
		NEED_FILE=yes
		;;

	delete)
		PROCEDURE=delete
		NEED_FILE=yes
		;;

	delget | delta)
		PROCEDURE=delta
		NEED_FILE=yes
		;;

	diffs)
		PROCEDURE=diffs
		NEED_FILE=yes
		;;

	edit)
		PROCEDURE=edit
		NEED_FILE=yes
		;;

	get)
		PROCEDURE=get
		NEED_FILE=yes
		;;

	log)
		PROCEDURE=log
		NEED_FILE=yes
		;;

	unedit)
		PROCEDURE=unedit
		NEED_FILE=yes
		;;

	*)
		abort "invalid cmd \"$CMD\""
		;;
	esac

	if [ "$NEED_FILE" = "yes" -a "$FILES" = "" ]
	then
		abort "\"$CMD\" with no files specified"
	fi

	echo "$PROCEDURE"
}

# ------------------------------------------------------------------------
# usage msg
#
usage ()
{
	echo "Type \"$PGM help\" for help" >&2
}

# ------------------------------------------------------------------------
# abort with a message
#
abort ()
{
	echo "$PGM: $1" >&2
	usage
	exit 1
}

# ------------------------------------------------------------------------
# help menu
#
help ()
{
	echo "usage: $PGM [options] <cmd> [file ...]"
	echo ""
	echo "options:"
	echo "  -i          recurse through sub-dirs [ info list refresh tell ]"
	echo "  -y comment  specify delta comment    [ create delete delta ]"
	echo "  -c client   login as client"
	echo ""
	echo "cmds that don't require a file:"
	echo "  help        display help"
	echo "  info        list info for files checked-out by anyone"
	echo "  list        list files in archive"
	echo "  rebuild     rebuild entire view of archive"
	echo "  refresh     sync with archive"
	echo "  sync        sync the entire view of the archive"
	echo "  tell        list files checked-out by client"
	echo ""
	echo "cmds that require a file:"
	echo "  create      add a file"
	echo "  delete      delete a file"
	echo "  delta       check-in a file"
	echo "  diffs       get differences in a file from the archive"
	echo "  edit        check-out a file for editing"
	echo "  get         refresh a file"
	echo "  log         display changes log for a file"
	echo "  unedit      throw away changes in a file"

	return 0
}

# ------------------------------------------------------------------------
# get comments
#
get_comments ()
{
	$DEBUG

	tty >$DEVNULL && echo "Enter comment lines (\".\" to quit):"

	touch $CMTFILE
	while read line
	do
		[ "$line" = "." ] && break
		echo "	$line" >>$CMTFILE
	done
}

# ------------------------------------------------------------------------
# create file
#
create ()
{
	$DEBUG

	if [ "$COMMENT" = "" ]
	then
		echo "new file" | get_comments
	else
		echo "$COMMENT" | get_comments
	fi

	(
		p4 $CLIENT change -o |
		sed -e '/<enter description here>/,$d'

		cat $CMTFILE
		echo ""
		echo "Files:"
		for file in $FILES
		do
			case "$file" in
			?:* | /* | .*)	;;
			*)		[ "$CWD" != "" ] && file="$CWD/$file"
					;;
			esac

			echo "	//depot/$file	# add"
		done
	) >$TMPFILE

	p4 $CLIENT add $FILES
	p4 $CLIENT submit -i <$TMPFILE
}

# ------------------------------------------------------------------------
# delete a file
#
delete ()
{
	$DEBUG

	if [ "$COMMENT" = "" ]
	then
		get_comments
	else
		echo "$COMMENT" | get_comments
	fi

	(
		p4 $CLIENT change -o |
		sed -e '/<enter description here>/,$d'

		cat $CMTFILE
		echo ""
		echo "Files:"
		for file in $FILES
		do
			case "$file" in
			?:* | /* | .*)	;;
			*)		[ "$CWD" != "" ] && file="$CWD/$file"
					;;
			esac

			echo "	//depot/$file	# delete"
		done
	) >$TMPFILE

	p4 $CLIENT delete $FILES
	p4 $CLIENT submit -i <$TMPFILE
}

# ------------------------------------------------------------------------
# check-in a file
#
delta ()
{
	$DEBUG

	if [ "$COMMENT" = "" ]
	then
		get_comments
	else
		echo "$COMMENT" | get_comments
	fi

	(
		p4 $CLIENT change -o |
		sed -e '/<enter description here>/,$d'

		cat $CMTFILE
		echo ""
		echo "Files:"
		for file in $FILES
		do
			case "$file" in
			?:* | /* | .*)	;;
			*)		[ "$CWD" != "" ] && file="$CWD/$file"
					;;
			esac

			echo "	//depot/$file	# edit"
		done
	) >$TMPFILE

	p4 $CLIENT submit -i <$TMPFILE
}

# ------------------------------------------------------------------------
# get diffs for a file
#
diffs ()
{
	$DEBUG

	p4 $CLIENT diff $FILES
}

# ------------------------------------------------------------------------
# check-out a file
#
edit ()
{
	$DEBUG

	(cd $ROOT; cms.sh -i info >$TMPFILE)

	OK=""
	for file in $FILES
	do
		line=`grep "^${file}	" $TMPFILE`
		if [ $? -eq 0 ]
		then
			who=`echo "$line" | cut -f2`
			echo "$PGM: $file already checked out by $who" >&2
		else
			OK="$OK $file"
		fi
	done
	[ "$OK" = "" ] && return 1

	p4 $CLIENT edit $OK
	p4 $CLIENT lock $OK >$DEVNULL 2>&1

	return 0
}

# ------------------------------------------------------------------------
# get a file
#
get ()
{
	$DEBUG

	p4 $CLIENT sync -f $FILES
}

# ------------------------------------------------------------------------
# display log for a file
#
log ()
{
	$DEBUG

	p4 $CLIENT changes -l -s submitted $FILES
}

# ------------------------------------------------------------------------
# unedit a file
#
unedit ()
{
	$DEBUG

	p4 $CLIENT revert $FILES

	for file in $FILES
	do
		d=`dirname  $file`
		f=`basename $file`
		case "$f" in
		*.c)	rm -f $d/`basename $f .c  `.o*;;
		*.cpp)	rm -f $d/`basename $f .cpp`.o*;;
		esac
	done

	return 0
}

# ------------------------------------------------------------------------
# refresh a directory
#
refresh ()
{
	$DEBUG

	list | p4 $CLIENT -x - sync -f
}

# ------------------------------------------------------------------------
# rebuild the view of the archive
#
rebuild ()
{
	$DEBUG

	p4 $CLIENT sync -f
}

# ------------------------------------------------------------------------
# sync the view with the archive
#
sync ()
{
	$DEBUG

	p4 $CLIENT sync
}

# ------------------------------------------------------------------------
# get info on checked-out files
#
info ()
{
	$DEBUG

	p4 $CLIENT opened -a 2>$DEVNULL |
	while read line
	do
		file=`echo "$line" |
			sed -e "s,.*$DEPOT,," -e "s,#.*,,"`
		who=`echo "$line" | sed -e "s,.*@,," -e "s, .*,,"`

		if [ "$RECURSE" = "yes" ]
		then
			if [ "$CWD" = "" ]
			then
				echo "$file	$who"
			else
				case "$file" in
				$CWD/*)
					file=`echo $file | sed -e "s,$CWD/,,"`
					echo "$file	$who"
					;;
				esac
			fi
		else
			if [ "$CWD" = "" ]
			then
				echo "$file	$who" | grep -v /
			else
				case "$file" in
				$CWD/*)
					file=`echo $file | sed -e "s,$CWD/,,"`
					echo "$file	$who" | grep -v /
					;;
				esac
			fi
		fi
	done

	return 0
}

# ------------------------------------------------------------------------
# list files
#
list ()
{
	$DEBUG

	ROOT=`p4 $CLIENT client -o |
		grep '^Root' | \
		sed -e 's/.*[ 	]//' -e 's,\\\\,/,'`

	if [ "$RECURSE" = "yes" ]
	then
		p4 $CLIENT have ... | sed -e "s,.*$ROOT.,," | sed -e '/^[^/]*$/s,^,./,'
	else
		p4 $CLIENT have ... | sed -e "s,.*$ROOT.,," | grep -v /
	fi | sort

	return 0
}

# ------------------------------------------------------------------------
# get list of checked-out files
#
tell ()
{
	$DEBUG

	p4 $CLIENT opened 2>$DEVNULL |
	sed -e "s,.*$DEPOT,,;s,#.*,," |
	while read file
	do
		if [ "$RECURSE" = "yes" ]
		then
			if [ "$CWD" = "" ]
			then
				echo "$file"
			else
				echo "$file" |
				grep "^$CWD/" |
				sed -e "s,$CWD/,,"
			fi
		else
			if [ "$CWD" = "" ]
			then
				echo "$file" |
				grep -v /
			else
				echo "$file" |
				grep "^$CWD/" |
				sed -e "s,$CWD/,," |
				grep -v /
			fi
		fi
	done

	return 0
}

# ------------------------------------------------------------------------
# get user's root directory in canonical form
#
get_root ()
{
	p4 $CLIENT client -o |
	sed -n -e "/^Root:/ {
			s/Root:[ 	]*//
			y,$ATOZ\\\\,$atoz/,
			p
		}"
}

# ------------------------------------------------------------------------
# get CWD in canonical form
#
get_cwd ()
{
	set | sed -n -e "/^PWD=/ {
				s:PWD=::
				s:\"::g
				y,$ATOZ,$atoz,
				s,$ROOT/*,,
				p
			}"
}

# ========================================================================
# main
#
PGM=`basename $0`	export PGM

# ------------------------------------------------------------------------
# setup tmpfile names & nullfile name
#
[ "$TMP" = "" ] && TMP=/tmp
TMPFILE=$TMP/cms$$.tmp	export TMPFILE
CMTFILE=$TMP/cms$$.cmt	export CMTFILE

if [ -c /dev/null ]
then
	DEVNULL=/dev/null
else
	DEVNULL=NUL:
fi
export DEVNULL

trap delete_temp_files 2

# ------------------------------------------------------------------------
# collect any options
#
RECURSE=no	export RECURSE
CLIENT=""	export CLIENT
COMMENT=""	export COMMENT

[ "$1" = "" ] && abort "no cmd given"

while [ "$1" != "" ]
do
	case "$1" in

	-D)
		DEBUG="set -x"	export DEBUG
		$DEBUG
		shift
		;;

	-i)
		RECURSE=yes
		shift
		;;

	-y)
		COMMENT="$2"	export COMMENT
		shift 2
		;;

	-c)
		CLIENT="-c $2"	export CLIENT
		shift 2
		;;

	'-?' | -help | --help)
		set -- help
		break
		;;

	-*)
		abort "invalid option \"$1\""
		;;

	*)
		break
		;;
	esac
done

# ------------------------------------------------------------------------
# now get cmd & file list
#
[ "$1" = "" ] && abort "no cmd given"

CMD="$1"	export CMD
shift

FILES="$*"	export FILES

# ------------------------------------------------------------------------
# get root of client's view & relative CWD
#
ROOT=`get_root`
CWD=`get_cwd`
DEPOT="//depot/"
export ROOT CWD DEPOT

# ------------------------------------------------------------------------
# check if valid cmd & execute it
#
eval `check_cmd`

# ------------------------------------------------------------------------
# final cleanup
#
STATUS=$?

delete_temp_files

exit $STATUS
