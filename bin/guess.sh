# ------------------------------------------------------------------------
#	guess - A script to guess the architecture we are on.
#
#	Rather than try to do some "configure" about specialized requirements
#	for a given platform, I think it is easier to just try to guess the
#	platform, and you usually know the configuration for that platform.
#
#	In my experience, you can usually determine a platform with a simple
#	test command, but it important to do the tests in the right order.
#	Therefore, we can't have the "test" for a platform be a part of the
#	"env" script (e.g. running "<platform>.env guess"), since we then have
#	no idea of the order the tests must be run in, unless we had a table
#	of entries.  And as long as we have to have a separate script/file for
#	this table, we might as well just put the tests here, since then, by
#	looking at the other tests, we can more easily determine where a
#	new test should be in the pecking order.
#
#	The table has the form:
#
#		platform	description
#
#	For each <platform>, we run the script "test_for_<platform>".
#	If no test script returns successfully, we exit with failure.
#	If a test script returns successfully, we output:
#
#		eval format:
#			plat_name="<platform-name>"
#			plat_desc="<platform-desc>"
#
#		tab format:
#			<platform-name>\t<platform_name>
#
#		else:
#			<platform-name>
#
#	and exit with success.
#

# ------------------------------------------------------------------------
# Usage
#
usage ()
{
	echo "usage: $PGM [options] [name]"
	echo "options:"
	echo "  -e      Output in eval format"
	echo "  -t      Output in tab  format"
	echo "  -n      Output in norm format (default)"
	echo "  -l      List all platforms"
	echo "name      Name to try"
}

# ------------------------------------------------------------------------
# platform test table
#
# In general, there *should* be one entry here for each <platform>.env
# file which we have.
#
# Remember: the order here is important!
#
platform_table ()
{
	sed -e '/^#/d' -e 's/		*/:/g' <<-EOF
		# ----------------------------------------------------------------
		#	platform	description
		#	--------	--------------------------
			cygwin		Windows (x86) (cygwin)
			winx86		Windows (x86)
			aix			IBM AIX (rs/6000 or PowerPC)
			alpha		Digital/ux (alpha)
			dg86		DG/ux (x86)
			dg88		DG/ux (88k)
			freebsd		FreeBSD (x86)
			openbsd		OpenBSD (x86)
			netbsd		NetBSD (x86)
			hp			HP/ux (pa/risc)
			i386v3		generic SVR3 (x86)
			i386v4		generic SVR4 (x86)
			linux		Linux (x86)
			ncr			ATT/NCR SVR4 (x86)
			osx			Mac OS/X (PowerPC)
			os5			SCO OpenServer (x86)
			sgi			IRIX (mips)
			solaris		Solaris (sparc)
			sunos		SunOS (sparc)
			sunx86		Solaris (x86)
			uw			SCO UnixWare (x86)
		EOF
}

# ------------------------------------------------------------------------
# platform-specific test routines
#
# If you don't know of any specific test for a platform, just
# always return false. (Remember 0 is true & 1 is false.)
#
test_for_cygwin ()
{
	[ "$OSTYPE" != "cygwin" ] && return 1

	return 0
}

test_for_winx86 ()
{
	[ "$PROCESSOR_REVISION" != "" -o -d "c:/" ] || return 1

	return 0
}

test_for_aix ()
{
	[ -d /lpp ] || return 1

	return 0
}

test_for_alpha ()
{
	[ -f /osf_boot ] || return 1

	return 0
}

test_for_dg86 ()
{
	[ -f /dgux -a -d /shlib ] || return 1

	return 0
}

test_for_dg88 ()
{
	[ -f /dgux  ] || return 1

	return 0
}

test_for_freebsd ()
{
	return 1
}

test_for_openbsd ()
{
	return 1
}

test_for_netbsd ()
{
	return 1
}

test_for_hp ()
{
	[ -f /stand/vmunix -a "`uname -r | grep 'B.1[012]'`" != "" ] || return 1

	return 0
}

test_for_i386v3 ()
{
	return 1
}

test_for_i386v4 ()
{
	return 1
}

test_for_linux ()
{
	[ -f /etc/lilo.conf ] || return 1

	return 0
}

test_for_ncr ()
{
	[ -f /stand/unix ] || return 1

	return 0
}

test_for_osx ()
{
	[ -f /mach_kernel ] || return 1

	return 0
}

test_for_os5 ()
{
	[ -d /tcb -a -d /sbin ] || return 1

	return 0
}

test_for_sgi ()
{
	return 1
}

test_for_solaris ()
{
	[ "`uname -sp`" = "SunOS sparc" ] || return 1

	return 0
}

test_for_sunos ()
{
	[ -f /vmunix ] || return 1

	return 0
}

test_for_sunx86 ()
{
	[ "`uname -sp`" = "SunOS intel" ] || return 1

	return 0
}

test_for_uw ()
{
	[ -f /stand/unix -a -f /stand/resmgr ] || return 1

	return 0
}

# ------------------------------------------------------------------------
# extract fields from a table entry
#
platform_fields ()
{
	# --------------------------------------------------------------------
	# set each field as a separate argument
	#
	SAVE="$IFS"
	IFS=":"
	set $1
	IFS="$SAVE"

	echo \
		"plat_name=\"$1\"" \
		"plat_desc=\"$2\""
}

# ------------------------------------------------------------------------
# list all platforms in the table
#
list_platforms ()
{
	platform_table |
	while read line
	do
		eval `platform_fields "$line"`

		plat_name=`echo "$plat_name                 " | cut -c1-16`
		echo "$plat_name $plat_desc"
	done
}

# ------------------------------------------------------------------------
# output the guessed result
#
output_platform ()
{
	fields="$1"
	eval $fields

	case "$MODE" in

	eval)	echo "$fields"					;;
	tab)	echo "$plat_name	$plat_desc"	;;
	*)		echo "$plat_name"				;;

	esac
}

# ------------------------------------------------------------------------
# try a platform name
#
try_name ()
{
	name="$1"
	line=`platform_table | grep "^$name:"` &&
	{
		fields=`platform_fields "$line"`
		eval $fields
		test_for_${plat_name} && output_platform "$fields"
	}
}

# ------------------------------------------------------------------------
# actual guess routine
#
# Note: some stoopid Unix shells can't break out of a loop in a pipe
# correctly, so we always at least finish the loop.
#
guess_platform ()
{
	found=no

	platform_table |
	while read line
	do
		[ "$found" = "yes" ] && continue

		fields=`platform_fields "$line"`
		eval $fields
		if test_for_${plat_name}
		then
			found=yes
			output_platform "$fields"
		fi
	done
}

# ========================================================================
# finally - the actual script
# ========================================================================

# ------------------------------------------------------------------------
# process any options
#
PGM=`basename $0`
MODE=norm
NAME=

while [ "$1" != "" ]
do
	case "$1" in

	-e)
		MODE=eval
		shift
		;;

	-t)
		MODE=tab
		shift
		;;

	-n)
		MODE=norm
		shift
		;;

	-l)
		list_platforms
		exit
		;;

	'-?' | -help | --help)
		usage
		exit
		;;

	-*)
		echo "$PGM: invalid arg: $1" >&2
		usage >&2
		exit 1
		;;

	*)
		NAME="$1"
		shift
		break
		;;
	esac
done

# ------------------------------------------------------------------------
# now do it
#
if [ "$NAME" != "" ]
then
	result=`try_name "$NAME"`
else
	result=`guess_platform`
fi

# ------------------------------------------------------------------------
# output result if we have one
# (note that this test also sets the return code)
#
[ "$result" != "" ] && echo "$result"
