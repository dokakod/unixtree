/*------------------------------------------------------------------------
 * services struct for ftp
 */

#ifndef FTPSVC_H
#define FTPSVC_H

#include <sys/types.h>
#include <time.h>

#if V_WINDOWS
#include <winsock.h>

/*------------------------------------------------------------------------
 * FTP reply codes
 */
#define PRELIM		1
#define COMPLETE	2
#define CONTINUE	3
#define TRANSIENT	4

/*------------------------------------------------------------------------
 * FTP type codes
 */
#define TYPE_A		1	/* ASCII */
#define TYPE_E		2	/* EBCDIC */
#define TYPE_I		3	/* image */
#define TYPE_L		4	/* local byte size */

/*------------------------------------------------------------------------
 * telnet codes
 */
#define	IAC			255		/* interpret as command: */
#define	DONT		254		/* you are not to use option */
#define	DO			253		/* please, you use option */
#define	WONT		252		/* I won't use option */
#define	WILL		251		/* I will use option */
#define	SB			250		/* interpret as subnegotiation */
#define	GA			249		/* you may reverse the line */
#define	EL			248		/* erase the current line */
#define	EC			247		/* erase the current character */
#define	AYT			246		/* are you there */
#define	AO			245		/* abort output--but let prog finish */
#define	IP			244		/* interrupt process--permanently */
#define	BREAK		243		/* break */
#define	DM			242		/* data mark--for connect. cleaning */
#define	NOP			241		/* nop */
#define	SE			240		/* end sub negotiation */
#define	EOR			239		/* end of record (transparent mode) */
#define	ABORT		238		/* Abort process */
#define	SUSP		237		/* Suspend process */
#define	xEOF		236		/* End of file: EOF is already used... */

#endif

#if V_UNIX
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#include <netdb.h>
#include <arpa/ftp.h>
#include <arpa/telnet.h>
#include <arpa/inet.h>
#include <unistd.h>

#define closesocket		os_close
#endif

/*------------------------------------------------------------------------
 * FTP_SVC struct
 */
struct ftp_svc
{
	struct servent		sp;
	struct sockaddr_in  data_addr;
	struct sockaddr_in  lcl_addr;
	struct sockaddr_in  rmt_addr;
};
typedef struct ftp_svc FTP_SVC;

#endif /* FTPSVC_H */
