/*------------------------------------------------------------------------
 * "diff" print routine (side-by-side)
 */
#include "diffsys.h"

/*------------------------------------------------------------------------
 * diff_expand_tabs() - expand tabs in a string
 */
static int diff_expand_tabs (const char *str, int tab_width, char *buf)
{
	const char *	s	= str;
	char *			b	= buf;
	int				i	= 0;

	for (; *s; s++)
	{
		int c = *(unsigned char *)s;

		if (iscntrl(c))
		{
			if (c == '\n' || c == '\r')
				continue;

			if (c != '\t')
			{
				*b++ = '.';
				i++;
			}
			else
			{
				*b++ = ' ';
				for (i++; (i % tab_width) != 0; i++)
					*b++ = ' ';
			}
		}
		else
		{
			*b++ = c;
			i++;
		}
	}
	*b = 0;

	return (b - buf);
}

static void print_line (DIFF_OUTPUT_RTN *rtn, void *data, DIFF_LINE *d,
	int page_width, int show_numbers, int tab_width)
{
	char	buffer[512];
	char	line[512];
	char *	b = buffer;
	int		type;
	int		hw;
	int		i;

	/*--------------------------------------------------------------------
	 * get line type
	 */
	switch (LINE_TYPE(d))
	{
	case LEFT:		type = '<';		break;
	case RIGHT:		type = '>';		break;
	case COMMON:	type = ' ';		break;
	case CHANGED:	type = '|';		break;

	default:		type = '?';		break;
	}

	/*--------------------------------------------------------------------
	 * get half-line width
	 */
	hw = (page_width - 2) / 2;
	if (show_numbers)
		hw -= 5;

	/*--------------------------------------------------------------------
	 * do left side
	 */
	if (show_numbers)
	{
		if (LINE_NUM_LF(d) > 0)
		{
			sprintf(b, "%4.4d", LINE_NUM_LF(d));
		}
		else
		{
			strcpy(b, "    ");
		}
		b += 4;
		*b++ = ' ';
	}

	diff_expand_tabs(LINE_PTR_LF(d), tab_width, line);
	for (i = 0; i < hw && line[i]; i++)
		*b++ = line[i];

	/*--------------------------------------------------------------------
	 * pad to gutter
	 */
	for (; i < hw; i++)
		*b++ = ' ';

	/*--------------------------------------------------------------------
	 * do gutter
	 */
	*b++ = ' ';
	*b++ = type;
	*b++ = ' ';

	/*--------------------------------------------------------------------
	 * do right side
	 */
	if (show_numbers)
	{
		if (LINE_NUM_RT(d) > 0)
		{
			sprintf(b, "%4.4d", LINE_NUM_RT(d));
		}
		else
		{
			strcpy(b, "    ");
		}
		b += 4;
		*b++ = ' ';
	}

	diff_expand_tabs(LINE_PTR_RT(d), tab_width, line);
	for (i = 0; i < hw && line[i]; i++)
		*b++ = line[i];

	/*--------------------------------------------------------------------
	 * now print it
	 */
	*b++ = '\n';
	*b   = 0;

	(*rtn)(data, buffer);
}

void diff_print_side (DIFF_OUTPUT_RTN *rtn, void *data, DIFF_LINE *lines,
	int changed, int page_width, int show_numbers, int tab_width)
{
	DIFF_LINE *d;

	for (d = lines; d; d = d->next)
	{
		if (changed && LINE_TYPE(d) == COMMON)
			continue;
		print_line(rtn, data, d, page_width, show_numbers, tab_width);
	}
}
