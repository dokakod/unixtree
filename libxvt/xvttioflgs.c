/*------------------------------------------------------------------------
 *	termio flag struct tables
 */

#include "xvtcommon.h"
#include "xvttioflgs.h"

/*------------------------------------------------------------------------
 * i-flags
 */
const TIOFLGS xvt_tio_iflag_words[] =
{
	{ "ignbrk",	IGNBRK,		0	},
	{ "brkint",	BRKINT,		0	},
	{ "ignpar",	IGNPAR,		0	},
	{ "parmrk",	PARMRK,		0	},
	{ "inpck",	INPCK,		0	},
	{ "istrip",	ISTRIP,		0	},
	{ "inlcr",	INLCR,		0	},
	{ "igncr",	IGNCR,		0	},
	{ "icrnl",	ICRNL,		0	},
#ifdef	IUCLC
	{ "iuclc",	IUCLC,		0	},
#endif
	{ "ixon",	IXON,		0	},
#ifdef	IXANY
	{ "ixany",	IXANY,		0	},
#endif
	{ "ixoff",	IXOFF,		0	},

	{ 0 }
};

/*------------------------------------------------------------------------
 * l-flags
 */
const TIOFLGS xvt_tio_lflag_words[] =
{
	{ "isig",	ISIG,		0	},
	{ "icanon",	ICANON,		0	},
#ifdef XCASE
	{ "xcase",	XCASE,		0	},
#endif
	{ "echo",	ECHO,		0	},
	{ "echoe",	ECHOE,		0	},
	{ "echok",	ECHOK,		0	},
	{ "echonl",	ECHONL,		0	},
	{ "noflsh",	NOFLSH,		0	},

	{ 0 }
};

/*------------------------------------------------------------------------
 * c-flags
 */
const TIOFLGS xvt_tio_cflag_words[] =
{
	{ "clocal",	CLOCAL,		0	},
	{ "cread",	CREAD,		0	},
	{ "cstopb",	CSTOPB,		0	},
	{ "hupcl",	HUPCL,		0	},
	{ "parenb",	PARENB,		0	},
	{ "parodd",	PARODD,		0	},

	{ "cs5",	CS5,		CSIZE	},
	{ "cs6",	CS6,		CSIZE	},
	{ "cs7",	CS7,		CSIZE	},
	{ "cs8",	CS8,		CSIZE	},

#ifdef CBAUD
	{ "B0",		B0,		CBAUD	},
	{ "B50",	B50,		CBAUD	},
	{ "B75",	B75,		CBAUD	},
	{ "B110",	B110,		CBAUD	},
	{ "B134",	B134,		CBAUD	},
	{ "B150",	B150,		CBAUD	},
	{ "B200",	B200,		CBAUD	},
	{ "B300",	B300,		CBAUD	},
	{ "B600",	B600,		CBAUD	},
	{ "B1200",	B1200,		CBAUD	},
	{ "B1800",	B1800,		CBAUD	},
	{ "B2400",	B2400,		CBAUD	},
	{ "B4800",	B4800,		CBAUD	},
	{ "B9600",	B9600,		CBAUD	},
#ifdef	B19200
	{ "B19200",	B19200,		CBAUD	},
#endif
#ifdef	B38400
	{ "B38400",	B38400,		CBAUD	},
#endif
#ifdef	B76800
	{ "B76800",	B76800,		CBAUD	},
#endif
#ifdef	B115200
	{ "B115200",	B115200,	CBAUD	},
#endif
#ifdef	B153600
	{ "B153600",	B153600,	CBAUD	},
#endif
#ifdef	B230400
	{ "B230400",	B230400,	CBAUD	},
#endif
#ifdef	B307200
	{ "B307200",	B307200,	CBAUD	},
#endif
#ifdef	B460800
	{ "B460800",	B460800,	CBAUD	},
#endif
#endif


#ifdef	RCV1EN
	{ "rcv1en",	RCV1EN,		0	},
#endif
#ifdef	XMT1EN
	{ "xmt1en",	XMT1EN,		0	},
#endif
#ifdef	LOBLK
	{ "loblk",	LOBLK,		0	},
#endif
#ifdef	XCLUDE
	{ "xclude",	XCLUDE,		0	},
#endif

	{ 0 }
};

/*------------------------------------------------------------------------
 * o-flags
 */
const TIOFLGS xvt_tio_oflag_words[] =
{
	{ "opost",	OPOST,		0	},
#ifdef OLCUC
	{ "olcuc",	OLCUC,		0	},
#endif
#ifdef ONLCR
	{ "onlcr",	ONLCR,		0	},
#endif
#ifdef OCRNL
	{ "ocrnl",	OCRNL,		0	},
#endif
#ifdef ONOCR
	{ "onocr",	ONOCR,		0	},
#endif
#ifdef ONLRET
	{ "onlret",	ONLRET,		0	},
#endif
#ifdef OFILL
	{ "ofill",	OFILL,		0	},
#endif
#ifdef OFDEL
	{ "ofdel",	OFDEL,		0	},
#endif
#if defined(NL1) && defined(NLDLY)
	{ "nl1",	NL1,		NLDLY	},
#endif
#if defined(CR1) && defined(CRDLY)
	{ "cr1",	CR1,		CRDLY	},
#endif
#if defined(CR2) && defined(CRDLY)
	{ "cr2",	CR2,		CRDLY	},
#endif
#if defined(CR3) && defined(CRDLY)
	{ "cr3",	CR3,		CRDLY	},
#endif
#if defined(TAB1) && defined(TABDLY)
	{ "tab1",	TAB1,		TABDLY	},
#endif
#if defined(TAB2) && defined(TABDLY)
	{ "tab2",	TAB2,		TABDLY	},
#endif
#if defined(TAB3) && defined(TABDLY)
	{ "tab3",	TAB3,		TABDLY	},
#endif
#if defined(BS1) && defined(BSDLY)
	{ "bs1",	BS1,		BSDLY	},
#endif
#if defined(VT1) && defined(VTDLY)
	{ "vt1",	VT1,		VTDLY	},
#endif
#if defined(FF1) && defined(FFDLY)
	{ "ff1",	FF1,		FFDLY	},
#endif

	{ 0 }
};

/*------------------------------------------------------------------------
 * cc-flags (canonical)
 */
const TIOFLGS xvt_tio_ccflag_words_can[] =
{
	{ "intr",	VINTR,		CINTR	},
	{ "quit",	VQUIT,		CQUIT	},
	{ "erase",	VERASE,		CERASE	},
	{ "kill",	VKILL,		CKILL	},
#if defined(VEOF)     && defined(CEOT)
	{ "eof",	VEOF,		CEOT	},
#endif
#if defined(VEOL)     && defined(CEOL)
	{ "eol",	VEOL,		CEOL	},
#endif
#if defined(VEOL2)    && defined(CEOL2)
	{ "eol2",	VEOL2,		CEOL2	},
#endif
#if defined(VSWTCH)   && defined(CSWTCH)
	{ "swtch",	VSWTCH,		CSWTCH	},
#endif
#if defined(VSTART)   && defined(CSTART)
	{ "start",	VSTART,		CSTART	},
#endif
#if defined(VSTOP)    && defined(CSTOP)
	{ "stop",	VSTOP,		CSTOP	},
#endif
#if defined(VSUSP)    && defined(CSUSP)
	{ "susp",	VSUSP,		CSUSP	},
#endif
#if defined(VDSUSP)   && defined(CDSUSP)
	{ "dsusp",	VDSUSP,		CDSUSP	},
#endif
#if defined(VREPRINT) && defined(CRPRNT)
	{ "reprint",	VREPRINT,	CRPRNT	},
#endif
#if defined(VDISCARD) && defined(CFLUSH)
	{ "discard",	VDISCARD,	CFLUSH	},
#endif
#if defined(VWERASE)  && defined(CWERASE)
	{ "werase",	VWERASE,	CWERASE	},
#endif
#if defined(VLNEXT)   && defined(CLNEXT)
	{ "lnext",	VLNEXT,		CLNEXT	},
#endif

	{ 0 }
};

/*------------------------------------------------------------------------
 * cc-flags (non-canonical)
 */
const TIOFLGS xvt_tio_ccflag_words_unc[] =
{
	{ "min",	VMIN,		0	},
	{ "time",	VTIME,		0	},

	{ 0 }
};

/*------------------------------------------------------------------------
 * baudrate table
 */
const TIOFLGS xvt_tio_baudrate_words[] =
{
#ifdef CBAUD
	{ "0",		B0,		0	},
	{ "50",		B50,		50	},
	{ "300",	B300,		300	},
	{ "600",	B600,		600	},
	{ "1200",	B1200,		1200	},
	{ "2400",	B2400,		2400	},
	{ "4800",	B4800,		4800	},
	{ "9600",	B9600,		9600	},
#ifdef	B19200
	{ "19200",	B19200,		19200	},
#endif
#ifdef	B38400
	{ "38400",	B38400,		38400	},
#endif
#endif

	{ 0 }
};
