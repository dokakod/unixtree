/*------------------------------------------------------------------------
 *
 *	This simple little program outputs a comment line; the argument
 *	is a combination of the indent level and the comment type.  The
 *	indent count (default = 1) is the number of tabstops that that
 *	we should indent, and the comment type is either C (the default),
 *	shell, or postscript.  Examples:
 *
 *		$ ic 0ps
 *
 *		% --------------------------------------------------------
 *		%
 *		%
 *
 *
 *		$ ic 4sh
 *						# ------------------------
 *						#
 *						#
 *	and so on.
 *
 *	After the (optional) indent count we can include the "=" char
 *	which turns the dash into an equal sign -- some of us use this
 *	as a "stronger" documentation delimiter. The default char is
 *	of course a dash.
 *
 *	You can also add a tab-width after everything else:
 *
 *		$ ic 3sh8
 *
 *	The default tab-width is 4.
 *
 *	This program is handy to use from "vi":
 *	e.g., to insert a comment line from "vi", you would use "!!ic".
 */
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define		MAXLINE		75

static void do_indent (int n)
{
	while (n-- > 0)
		putchar('\t');
}

static void output (const char *s)
{
	for (; *s; s++)
		putchar(*s);
}

int main (int argc, char **argv)
{
	const char *	precomment	= "/*";		/* C 1st comment line	*/
	const char *	midcomment	= " *\n";	/* C 2nd comment line	*/
	const char *	postcomment	= " */\n";	/* C 3rd comment line	*/

	int				dash_char	= '-';		/* char to use for line	*/
	int				tabwidth	= 4;		/* tab width			*/
	int				n_indent	= 1;		/* number of indents	*/
	int				c_col		= 1;		/* current column		*/

	if (argc > 1)
	{
		char *	arg = argv[1];

		/*-----------------------------------------------------------
		 * check for help
		 */
		if (strcmp(arg, "-?")     == 0 ||
		    strcmp(arg, "-help")  == 0 ||
		    strcmp(arg, "--help") == 0)
		{
			printf("usage: %s [<indent>][=][cc|pp|sh|ps][<tab>]\n",
				argv[0]);
			return (0);
		}

		/*-----------------------------------------------------------
		 * collect indent (if present)
		 */
		if (isdigit(*arg))
		{
			n_indent = 0;
			while (isdigit(*arg))
				n_indent = (n_indent * 10) + (*arg++ - '0');
		}

		/*-----------------------------------------------------------
		 * get line type (if present)
		 */
		if (*arg == '=')
		{
			arg++;
			dash_char = '=';
		}

		/*-----------------------------------------------------------
		 * get comment type (if present)
		 */
		if (strncmp(arg, "cc", 2) == 0)
		{
			arg += 2;
		}
		else if (strncmp(arg, "sh", 2) == 0)
		{
			precomment  = "# ";			/* shell 1st comment line	*/
			midcomment  = "#\n";		/* shell 2nd comment line	*/
			postcomment = "#\n";		/* shell 3rd comment line	*/

			arg += 2;
		}
		else if (strncmp(arg, "ps", 2) == 0)
		{
			precomment  = "% ";			/* PostScript 1st comment line	*/
			midcomment  = "%\n";		/* PostScript 2nd comment line	*/
			postcomment = "%\n";		/* PostScript 3rd comment line	*/

			arg += 2;
		}
		else if (strncmp(arg, "pp", 2) == 0)
		{
			precomment  = "//";			/* C++ 1st comment line */
			midcomment  = "//\n";		/* C++ 2nd comment line */
			postcomment = "//\n";		/* C++ 3rd comment line */

			arg += 2;
		}

		/*-----------------------------------------------------------
		 * collect new tabwidth (if present)
		 */
		if (isdigit(*arg))
		{
			tabwidth = 0;
			while (isdigit(*arg))
				tabwidth = (tabwidth * 10) + (*arg++ - '0');
		}
	}

	/*---------------------------------------------------------------
	 * first take care of outputing the tabs for the initial indent.
	 */
	do_indent(n_indent);
	c_col += (n_indent * tabwidth);

	/*---------------------------------------------------------------
	 * Now start the comment in the traditional way.  Note that we
	 * have to increment the column counter while we do this...
	 */
	output(precomment);
	c_col += strlen(precomment);

	/*---------------------------------------------------------------
	 * now write the dashes that run across the whole line.
	 */
	while (c_col++ < MAXLINE)
		putchar(dash_char);

	/*---------------------------------------------------------------
	 * finish it off with a newline.
	 */
	putchar('\n');

	/*---------------------------------------------------------------
	 * Now output the second and third lines in the appropriate format
	 */
	do_indent(n_indent);
	output(midcomment);

	do_indent(n_indent);
	output(postcomment);

	return (0);
}
