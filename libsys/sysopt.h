/*------------------------------------------------------------------------
 *  Routines to return options from a command line.
 *
 *  getopt returns the next option letter in argv that
 *  matches a letter in optstr.
 *
 *  usage:	getopt     (argc, argv, optstr)
 *			getopt_ext (argc, argv, optstr, switches)
 *
 *  argc, argv are the command line arguments from main.
 *
 *  optstr is a string of recognized option letters.
 *	  If a letter is followed by a colon, the option is expected to
 *	  have an argument which may or may not be separated by white space.
 *	  A special case of # (with or without a colon) as an
 *	  option will allow a number by itself as an argument.
 *
 *  switches is a string containing switches to recognize.
 *	  UNIX standard is "-" or sometimes "-+"
 *	  DOS standard(?) is "/"
 *	getopt() uses a switches of "-".
 *
 *  "optarg" is set to point to the start of the option string
 *	  if an argument is expected.
 *
 *  "optind" contains the argv index of the next argument to be processed.
 *
 *  "optopt" will contain the character found, even if getopt() signals
 *	  an error.
 *
 *  "optsw"  will contain the option switch character encountered.
 *
 *  getopt() returns the letter of the next option found or OPT_ERR_EOF
 *	  if all options are found.
 *
 *	  Option processing ends with the end of the argument list
 *	  or with the first argument that does not start with <switch-char>.
 *	  The special option <switch-char><switch-char> may be used to
 *	  delimit the end of options; OPT_ERR_EOF will be returned and the
 *	  option will be skipped. This will enable a real argument
 *	  to also start with a <switch-char>.
 *	  An argument of <switch-char> by itself will also terminate the
 *	  option scan and return OPT_ERR_EOF, but the argument is not skipped.
 *
 *	  If getopt returns OPT_ERR_INV, this indicates an illegal option
 *	  was encountered.  The actual char will be in optopt.
 *
 *	  If getopt returns OPT_ERR_ARG, this indicates the option found
 *	  requires as argument which was not found.
 *
 *	If the variable "vopterr" is set to 0, no error msgs will be printed.
 *
 *------------------------------------------------------------------------
 *
 *  This routine is based on the UNIX routine getopt()
 *  with the following changes:
 *
 *		The routine is passed the switch chars to look for.
 *
 *		The external variable "optsw" contains the switch char found.
 *
 *		The external variable "optopt" contains the actual char
 *		found in the event of an error return.
 *
 *		The pseudo-option char # allows a number to be used as an option.
 *
 *		getopt() returns OPT_ERR_INV (-2) as an error indicator,
 *		whereas getopt() returns '?'.  This allows easier use of '?' as
 *		an actual option (usually for help).
 *
 *		getopt() returns OPT_ERR_ARG (-3) as an error if an argument
 *		requires an option which is not found, whereas getopt() prints
 *		a message to stderr.
 */
#ifndef SYSOPT_H
#define SYSOPT_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * defines for special chars in the opt-string
 */
#define OPT_ARG_XPTD_C	':'			/* indicator for option arg		*/
#define OPT_ARG_NUM_C	'#'			/* special code for numbers		*/

#define OPT_ARG_XPTD_S	":"			/* indicator for option arg		*/
#define OPT_ARG_NUM_S	"#"			/* special code for numbers		*/

/*------------------------------------------------------------------------
 * return values
 */
#define OPT_ERR_EOF		-1			/* error - at end of options	*/
#define OPT_ERR_INV		-2			/* error - invalid option char	*/
#define OPT_ERR_ARG		-3			/* error - arg expected			*/

/*------------------------------------------------------------------------
 * get next option from command line
 *
 * Note: this call will print a msg to stderr if an error was found.
 */
extern	int		sysopt_getopt		(int argc, char * const *argv,
										const char *optstr);

/*------------------------------------------------------------------------
 * get next option from command line (specifying switches to use)
 */
extern	int		sysopt_getopt_ext	(int argc, char * const *argv,
										const char *optstr,
										const char *switches);

/*------------------------------------------------------------------------
 * get error message from last getopt() call
 */
extern char *	sysopt_getopt_msg	(void);

/*------------------------------------------------------------------------
 * reset to start of cmd line
 */
extern void		sysopt_reset		(void);

/*------------------------------------------------------------------------
 * external data
 */
extern char *	sysopt_optarg;
extern int		sysopt_optind;
extern int		sysopt_optopt;
extern int		sysopt_opterr;
extern int		sysopt_optsw;

/*------------------------------------------------------------------------
 * User defines.  These are the "names" users should use.
 */
#define optarg		sysopt_optarg
#define optind		sysopt_optind
#define optopt		sysopt_optopt
#define opterr		sysopt_opterr
#define optsw		sysopt_optsw

#define getopt		sysopt_getopt
#define getopt_ext	sysopt_getopt_ext
#define getopt_msg	sysopt_getopt_msg

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* SYSOPT_H */
