/*------------------------------------------------------------------------
 * usage display
 */
#include "libprd.h"

void usage (int help_count, const char *pgm)
{
	char msgbuf[128];

	if (help_count == 0)
	{
		/*----------------------------------------------------------------
		 * error message
		 */
		sprintf(msgbuf, msgs(m_usage_us1), pgm);
		fprintf(stderr, "%s\n", msgbuf);
	}
	else
	{
		char	node_to_log[MAX_PATHLEN];
		int		xvt_avail = gbl(xvt_avail);

		/*----------------------------------------------------------------
		 * display help
		 */
		sprintf(msgbuf, msgs(m_usage_fmu), package_name());
		fprintf(stdout, "%s: %s\n", pgm, msgbuf);
		fprintf(stdout, "\n");

		if (xvt_avail)
			sprintf(msgbuf, msgs(m_usage_us2x), pgm);
		else
			sprintf(msgbuf, msgs(m_usage_us2),  pgm);
		fprintf(stdout, "%s\n", msgbuf);
		fprintf(stdout, "%s\n", msgs(m_usage_opt));
		fprintf(stdout, "\n");

		fprintf(stdout, "  %s\n", msgs(m_usage_a));
		fprintf(stdout, "  %s\n", msgs(m_usage_c));
		fprintf(stdout, "  %s\n", msgs(m_usage_k));
		fprintf(stdout, "  %s\n", msgs(m_usage_m));
		fprintf(stdout, "  %s\n", msgs(m_usage_n));
		fprintf(stdout, "  %s\n", msgs(m_usage_r));
		fprintf(stdout, "  %s\n", msgs(m_usage_v));
		if (xvt_avail)
		fprintf(stdout, "  %s\n", msgs(m_usage_w));
		fprintf(stdout, "  %s\n", msgs(m_usage_x));

		fprintf(stdout, "  %s\n", msgs(m_usage_e));
		fprintf(stdout, "  %s\n", msgs(m_usage_l));
		fprintf(stdout, "  %s\n", msgs(m_usage_t));

		if (help_count > 1)
		{
			int i;

			/*------------------------------------------------------------
			 * display helpall
			 */
			fprintf(stdout, "\n");
			fprintf(stdout, "  %s\n", msgs(m_usage_dd));
			fprintf(stdout, "  %s\n", msgs(m_usage_kk));
			fprintf(stdout, "  %s\n", msgs(m_usage_mm));

			fprintf(stdout, "  %s\n", msgs(m_usage_ff));
			fprintf(stdout, "  %s\n", msgs(m_usage_hh));
			fprintf(stdout, "  %s\n", msgs(m_usage_ii));
			fprintf(stdout, "  %s\n", msgs(m_usage_oo));
			fprintf(stdout, "  %s\n", msgs(m_usage_ss));

			/* -T options */

			fprintf(stdout, "\n");
			fprintf(stdout, "  %s\n", msgs(m_usage_tt));
			fprintf(stdout, "  %s\n", msgs(m_usage_tt_val));
			for (i=0; ; i++)
			{
				const char *	name = get_term_db_name(i);
				const char *	desc = get_term_db_desc(i);

				if (name == 0)
					break;

				sprintf(msgbuf, msgs(m_usage_tt_typ), name, desc);
				fprintf(stdout, "  %s\n", msgbuf);
			}
			sprintf(msgbuf, msgs(m_usage_tt_def), get_term_db());
			fprintf(stdout, "  %s\n", msgbuf);

			/* -X options */

			if (xvt_avail)
			{
				fprintf(stdout, "\n");
				fprintf(stdout, "  %s\n", msgs(m_usage_xx));
			}

			/* -Z options */

			fprintf(stdout, "\n");
			fprintf(stdout, "  %s\n", msgs(m_usage_zz));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzcc));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzdd));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzff));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzmm));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzkk));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzk));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzss));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzs));
			fprintf(stdout, "  %s\n", msgs(m_usage_zzvv));
		}

		fprintf(stdout, "\n");
		strcpy(node_to_log, opt(default_node));
		fn_resolve_pathname(node_to_log);
		if (strcmp(node_to_log, opt(default_node)) == 0)
		{
			sprintf(msgbuf, msgs(m_usage_nodmsg_2), opt(default_node));
		}
		else
		{
			sprintf(msgbuf, msgs(m_usage_nodmsg_1), opt(default_node),
				node_to_log);
		}
		fprintf(stdout, "%s\n", msgbuf);

		if (xvt_avail)
		{
			fprintf(stdout, "\n");
			xvt_tk_help(gbl(xvt_data), stdout, XVT_TK_HELP_SHORT,
				XVT_TK_HELP_HERE);
			sprintf(msgbuf, msgs(m_usage_xvt), pgm);
			fprintf(stdout, "%s\n", msgbuf);
		}
	}
}
