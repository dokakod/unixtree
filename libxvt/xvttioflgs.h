/*------------------------------------------------------------------------
 *	header for termio flags struct
 */
#ifndef XVTTIOFLGS_H
#define XVTTIOFLGS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * termio settings for sane
 */
#ifdef IXANY
#define	TIO_ISANE	( BRKINT | IGNPAR | ISTRIP | ICRNL | IXON  | IXANY )
#else
#define	TIO_ISANE	( BRKINT | IGNPAR | ISTRIP | ICRNL | IXON  )
#endif
#ifdef ONLCR
#define	TIO_OSANE	( OPOST  | ONLCR  )
#else
#define	TIO_OSANE	( OPOST  )
#endif
#define	TIO_CSANE	( CS8    | CREAD  | HUPCL  )
#define	TIO_LSANE	( ISIG   | ICANON | ECHO   | ECHOE | ECHOK )

#define TIO_SANE	"SANE"

/*------------------------------------------------------------------------
 * This is the structure of the table of our possible stty flag words.
 * For each possible word, we look at the masked-off bits and see if the
 * result matches the given mode; if so, we have a match.  As a special
 * case, the mask can be zero which means that the mode bits themselves
 * should be used as the mask.
 *
 * Most of the bits in the words are just by themselves (i.e., CLOCAL),
 * so we should just check for them directly.  Others such as the bits for
 * the speed need to be masked with CBAUD and checked for equality.
 */
struct tioflgs
{
	const char *	string;		/* flag name */
	unsigned int	mode;		/* flag value */
	unsigned int	mask;		/* mask to use */
};
typedef struct tioflgs TIOFLGS;

/*------------------------------------------------------------------------
 * extern termio flag tables
 */
extern const TIOFLGS xvt_tio_iflag_words[];			/*  i-flags				*/
extern const TIOFLGS xvt_tio_lflag_words[];			/*  l-flags				*/
extern const TIOFLGS xvt_tio_cflag_words[];			/*  c-flags				*/
extern const TIOFLGS xvt_tio_oflag_words[];			/*  o-flags				*/
extern const TIOFLGS xvt_tio_ccflag_words_can[];	/* cc-flags (canon)		*/
extern const TIOFLGS xvt_tio_ccflag_words_unc[];	/* cc-flags (non-canon)	*/

/*------------------------------------------------------------------------
 * extern termio baudrate table
 */
extern const TIOFLGS xvt_tio_baudrate_words[];

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* XVTTIOFLGS_H */
