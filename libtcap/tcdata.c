/*------------------------------------------------------------------------
 * termcap data routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_tc_bool_val() - internal routine to evaluate a bool entry
 */
int tcap_tc_bool_val (const char *str)
{
	/*--------------------------------------------------------------------
	 * skip over opening quote
	 */
	if (*str == '"')
		str++;

	/*--------------------------------------------------------------------
	 * if empty, just return false
	 */
	if (*str == 0 || *str == '"')
		return (FALSE);

	/*--------------------------------------------------------------------
	 * check if alpha or numeric
	 */
	if (isdigit(*str))
	{
		/*----------------------------------------------------------------
		 * numeric - return numeric value
		 */
		return atoi(str);
	}
	else
	{
		/*----------------------------------------------------------------
		 * alpha - check against keywords
		 */
		if (tcap_trmncmp("on",    str) == 0)	return (TRUE);
		if (tcap_trmncmp("yes",   str) == 0)	return (TRUE);
		if (tcap_trmncmp("true",  str) == 0)	return (TRUE);

		if (tcap_trmncmp("off",   str) == 0)	return (FALSE);
		if (tcap_trmncmp("no",    str) == 0)	return (FALSE);
		if (tcap_trmncmp("false", str) == 0)	return (FALSE);
	}

	/*--------------------------------------------------------------------
	 * unknown entries are treated as false
	 */
	return (FALSE);
}

/*------------------------------------------------------------------------
 * tcap_tc_find_entry() - get index of termcap entry for a given name
 */
const TC_DATA * tcap_tc_find_entry (const char *name)
{
	const TC_DATA *tc;

	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		if (tcap_trmcmp(name, tc->trmc_label) == 0)
			return (tc);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tc_find_cmd() - get index of termcap entry for a given cmd
 */
const TC_DATA * tcap_tc_find_cmd (SCRN_CMD cmd)
{
	const TC_DATA *tc;

	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		if (tc->trmc_cmd == cmd)
			return (tc);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tc_set_entry() - store a value into the nth entry of a table
 *
 * Note: value is always a string, which is converted if needed
 */
int tcap_tc_set_entry (TCAP_DATA *td, const TC_DATA *tc, const char *value)
{
	char **	cp;
	int *	ip;
	int *	bp;

	/*--------------------------------------------------------------------
	 * set according to data type
	 */
	switch (tc->trmc_flags & TC_TYPE)
	{
	case TC_NUM:
		/*----------------------------------------------------------------
		 * numeric entry
		 */
		ip = TCAP_TC_NUM_SP(td, tc);

		/*----------------------------------------------------------------
		 * convert to number (empty strings -> 0)
		 */
		if (value == 0 || *value == 0)
		{
			*ip = 0;
		}
		else
		{
			if (*value == '"')
				value++;
			*ip = atoi(value);
		}
		break;

	case TC_BLN:
		/*----------------------------------------------------------------
		 * boolean entry
		 */
		bp = TCAP_TC_BLN_SP(td, tc);

		/*----------------------------------------------------------------
		 * convert to boolean value
		 */
		*bp = (tcap_tc_bool_val(value) != 0);
		break;

	case TC_STR:
	case TC_PRM:
		/*----------------------------------------------------------------
		 * string entry
		 */
		cp = TCAP_TC_STR_SP(td, tc);

		/*----------------------------------------------------------------
		 * free any existing string
		 */
		if (is_cmd_pres(*cp))
		{
			FREE(*cp);
			*cp = 0;
		}

		/*----------------------------------------------------------------
		 * string may be:
		 *
		 *	NULL		leave NULL
		 *	empty		set to ERR
		 *	string		set entry to string
		 *	cap-name	copy capability string
		 */
		if (value == 0)
		{
		}
		else if (*value == 0)
		{
			*cp = (char *)(-1);
		}
		else if (*value == '"')
		{
			unsigned char conv_str[64];

			tcap_trans_term_str(value, conv_str, sizeof(conv_str));
			*cp = tcap_trm_dup_str((char *)conv_str);
		}
		else
		{
			const TC_DATA *	t;
			char **			vp;

			t = tcap_tc_find_entry(value);
			if (t == 0)
			{
				return (-1);
			}

			if ((t->trmc_flags & TC_TYPE) != TC_STR)
			{
				return (-1);
			}

			vp = TCAP_TC_STR_SP(td, t);

			if (is_cmd_pres(*vp))
				*cp = tcap_trm_dup_str(*vp);
		}
		break;

	default:
		return (-1);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tc_free - clear termcap array, freeing any malloced entries
 */
void tcap_tc_free (TCAP_DATA *td)
{
	const TC_DATA *	tc;

	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		char **	cp;
		int *	ip;
		int *	bp;

		switch (tc->trmc_flags & TC_TYPE)
		{
		case TC_NUM:
			ip = TCAP_TC_NUM_SP(td, tc);
			*ip = 0;
			break;

		case TC_BLN:
			bp = TCAP_TC_BLN_SP(td, tc);
			*bp = FALSE;
			break;

		case TC_STR:
		case TC_PRM:
			cp = TCAP_TC_STR_SP(td, tc);
			if (is_cmd_pres(*cp))
				FREE(*cp);
			*cp = 0;
			break;
		}
	}
}

/*------------------------------------------------------------------------
 * tcap_tc_get_cmd() - get name associated with a cmd enum
 */
const char * tcap_tc_get_cmd (const TCAP_DATA *td, SCRN_CMD cmd)
{
	const TC_DATA *		tc;

	/*--------------------------------------------------------------------
	 * look through tcap-name table
	 */
	for (tc=tcap_termcap_data; tc->trmc_label; tc++)
	{
		if (tc->trmc_cmd == cmd)
			return (tc->trmc_label);
	}

	/*--------------------------------------------------------------------
	 * If not there, it might be a pseudo-parameter id.  Look there.
	 */
	{
		const PARAM_INFO *	pi;

		/*----------------------------------------------------------------
		 * find any param entry with this cmd in its "use" spot
		 */
		for (pi=tcap_param_info; pi->cmd > 0; pi++) 
		{ 
			if (pi->use == cmd)
				break;
		}
	
		/*----------------------------------------------------------------
		 * find parm-data entry for this cmd
		 *
		 * Note that all param-entries with the same "use" point to
		 * the same parm-data entry.
		 */
		if (pi->cmd > 0)
		{
			const PARM_DATA *	pd	= &td->pdata;
			const PARM_ENTRY *	pe	= (PARM_ENTRY *)((char *)pd + pi->offs);

			if (pe->cmd > 0)
				return tcap_tc_get_cmd(td, pe->cmd);
		}

		/*----------------------------------------------------------------
		 * We special case FG & BG, since they *may* be overridden by
		 * specific FG & BG color entries. If the user used all color entries,
		 * then FG & BG may be empty.
		 */
		switch (cmd)
		{
		case S_FG:	return ("fg");
		case S_BG:	return ("bg");
		}
	}

	return (0);
}
