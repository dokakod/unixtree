# ------------------------------------------------------------------------
# script to substitute env var values for tokens in text files
#
# If the "-d name" option is specified, then the following is done:
#
#	1.	All lines between "+++<name>+++" and "---<name>---" are deleted
#		(including the start & stop lines).
#
#	2.	All other lines in the form "+++<...>+++" and "---<...>---"
#		are deleted.
#
# If "-l lang" is specified, the vars are loaded from the named resource file
#

# ------------------------------------------------------------------------
# usage
#
usage ()
{
	echo "usage: $PGM [-d name] [-l lang] [text-file]"
}

# ------------------------------------------------------------------------
# check arguments
#
PGM=$0

ARGS="`getopt "h:d:l:?" $*`"
[ $? -ne 0 ] && { usage >&2; exit 1; }

set -- $ARGS

NAME="NO-NAME"
LNGF=""
FILE="-"
while [ x"$1" != x"" ]
do
	case "$1" in

	'-?' | -h)
		usage
		exit 0
		;;

	-d)
		NAME="$2"
		shift 2
		;;

	-l)
		LNGF="$2"
		shift 2
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

[ x"$1" != x"" ] && FILE="$1"

# ------------------------------------------------------------------------
# check if vars should be loaded
#
if [ x"$LNGF" != x"" ]
then
	RESTOOL=$SRC_DIR/libres/restool
	RESFILE=$SRC_DIR/libres/$LNGF/$LNGF.res

	PACKAGE=`    $RESTOOL -p pgmi:m_pgm_package  $RESFILE`
	PROGRAM=`    $RESTOOL -p pgmi:m_pgm_program  $RESFILE`
	VARPRFX=`    $RESTOOL -p pgmi:m_pgm_varprfx  $RESFILE`
	SLOGAN=`     $RESTOOL -p pgmi:m_pgm_slogan   $RESFILE`
	VENDOR=`     $RESTOOL -p pgmi:m_pgm_vendor   $RESFILE`
	COPYRTE=`    $RESTOOL -p pgmi:m_pgm_copyrte  $RESFILE`
	EMLADDR=`    $RESTOOL -p pgmi:m_pgm_emladdr  $RESFILE`
	WWWADDR=`    $RESTOOL -p pgmi:m_pgm_wwwaddr  $RESFILE`
	FTPADDR=`    $RESTOOL -p pgmi:m_pgm_ftpaddr  $RESFILE`
	VERSION=`    $RESTOOL -p pgmi:m_pgm_version  $RESFILE`
	RELEASE=`    $RESTOOL -p pgmi:m_pgm_release  $RESFILE`
	BUILD=`      $RESTOOL -p pgmi:m_pgm_build    $RESFILE`
	BUILDER_NAM=`$RESTOOL -p pgmi:m_pgm_bldr_nam $RESFILE`
	BUILDER_EMA=`$RESTOOL -p pgmi:m_pgm_bldr_ema $RESFILE`
	BUILDER_ENV=`$RESTOOL -p pgmi:m_pgm_bldr_env $RESFILE`
fi

# ------------------------------------------------------------------------
# now do it
#
CWD=`pwd`

sed \
	-e "s|\${PKG}|$PACKAGE|g" \
	-e "s|\${PGM}|$PROGRAM|g" \
	-e "s|\${PRF}|$VARPRFX|g" \
	-e "s|\${SLG}|$SLOGAN|g" \
	-e "s|\${VND}|$VENDOR|g" \
	-e "s|\${CPY}|$COPYRTE|g" \
	-e "s|\${EMA}|$EMLADDR|g" \
	-e "s|\${WWW}|$WWWADDR|g" \
	-e "s|\${FTP}|$FTPADDR|g" \
	-e "s|\${VER}|$VERSION|g" \
	-e "s|\${REL}|$RELEASE|g" \
	-e "s|\${BLD}|$BUILD|g" \
	-e "s|\${BNM}|$BUILDER_NAM|g" \
	-e "s|\${BEM}|$BUILDER_EMA|g" \
	-e "s|\${BNV}|$BUILDER_ENV|g" \
	-e "s|\${INS}|$INS_DIR|g" \
	-e "s|\${CWD}|$CWD|g" \
	-e "/^+++${NAME}+++/,/^---${NAME}---/d" \
	-e "/^[+-][+-][+-][A-Z]/d" \
	$FILE
