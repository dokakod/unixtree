/*------------------------------------------------------------------------
 * "diff" print routine (std diff format)
 */
#include "diffsys.h"

static void print_range (DIFF_OUTPUT_RTN *rtn, void *data,
	DIFF_LINE *d_beg, DIFF_LINE *d_end, int lf_prev, int rt_prev,
	LINE_CLASS type)
{
	char	buf[80];
	char *	b = buf;

	switch (type)
	{
	case LEFT:
		b += sprintf(b, "%d", LINE_NUM_LF(d_beg));
		if (d_beg != d_end)
		{
			*b++ = ',';
			b += sprintf(b, "%d", LINE_NUM_LF(d_end));
		}

		*b++ = 'd';

		b += sprintf(b, "%d", rt_prev);
		break;

	case RIGHT:
		b += sprintf(b, "%d", lf_prev);

		*b++ = 'a';

		b += sprintf(b, "%d", LINE_NUM_RT(d_beg));
		if (d_beg != d_end)
		{
			*b++ = ',';
			b += sprintf(b, "%d", LINE_NUM_RT(d_end));
		}
		break;

	case CHANGED:
		b += sprintf(b, "%d", LINE_NUM_LF(d_beg));
		if (d_beg != d_end)
		{
			*b++ = ',';
			b += sprintf(b, "%d", LINE_NUM_LF(d_end));
		}

		*b++ = 'c';

		b += sprintf(b, "%d", LINE_NUM_RT(d_beg));
		if (d_beg != d_end)
		{
			*b++ = ',';
			b += sprintf(b, "%d", LINE_NUM_RT(d_end));
		}
		break;
	}

	*b++ = '\n';
	*b   = 0;

	(*rtn)(data, buf);
}

static void print_lines (DIFF_OUTPUT_RTN *rtn, void *data,
	DIFF_LINE *d_beg, DIFF_LINE *d_end, int left)
{
	char		buf[2048];
	DIFF_LINE *	d;

	buf[0] = left ? '<' : '>';
	buf[1] = ' ';

	for (d = d_beg; d; d = d->next)
	{
		if (left)
			strcpy(buf + 2, LINE_PTR_LF(d));
		else
			strcpy(buf + 2, LINE_PTR_RT(d));

		(*rtn)(data, buf);

		if (d == d_end)
			break;
	}
}

static void print_list (DIFF_OUTPUT_RTN *rtn, void *data,
	DIFF_LINE *d_beg, DIFF_LINE *d_end, int lf_prev, int rt_prev,
	LINE_CLASS type)
{
	switch (type)
	{
	case COMMON:
		return;

	case LEFT:
		print_range(rtn, data, d_beg, d_end, lf_prev, rt_prev, type);
		print_lines(rtn, data, d_beg, d_end, TRUE);
		break;

	case RIGHT:
		print_range(rtn, data, d_beg, d_end, lf_prev, rt_prev, type);
		print_lines(rtn, data, d_beg, d_end, FALSE);
		break;

	case CHANGED:
		print_range(rtn, data, d_beg, d_end, lf_prev, rt_prev, type);
		print_lines(rtn, data, d_beg, d_end, TRUE);
		(*rtn)(data, "---\n");
		print_lines(rtn, data, d_beg, d_end, FALSE);
		break;
	}
}

void diff_print_diff (DIFF_OUTPUT_RTN *rtn, void *data, DIFF_LINE *lines)
{
	DIFF_LINE *	dp_beg	= 0;
	DIFF_LINE *	dp_this	= 0;
	DIFF_LINE *	dp_prev	= 0;
	LINE_CLASS	lc_this	= (LINE_CLASS)0;
	LINE_CLASS	lc_prev	= (LINE_CLASS)0;
	int			lf_prev	= 0;
	int			rt_prev	= 0;

	for (dp_this = lines; dp_this; dp_this = dp_this->next)
	{
		lc_this	= LINE_TYPE(dp_this);

		if (lc_this != lc_prev)
		{
			if (dp_beg != 0)
			{
				print_list(rtn, data, dp_beg, dp_prev, lf_prev,
					rt_prev, lc_prev);
			}

			dp_beg	= dp_this;
			lc_prev	= lc_this;

			if (dp_prev != 0)
			{
				lf_prev = LINE_NUM_LF(dp_prev);
				rt_prev = LINE_NUM_RT(dp_prev);
			}
		}

		dp_prev = dp_this;
	}

	print_list(rtn, data, dp_beg, dp_prev, lf_prev, rt_prev, lc_prev);
}
