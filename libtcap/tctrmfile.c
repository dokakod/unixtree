/*------------------------------------------------------------------------
 * term-file routines
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * tcap_process_trm_file() - process a term file
 *
 * returns:
 *	-1	no term-file found
 *	 0	OK
 *	 1	errors in term-file
 */
int tcap_process_trm_file (TERMINAL *tp, const char *term,
	const char *srch_path, int flag, TERM_LIST *term_list)
{
	TCAP_DATA *			td		= tp->tcap;
	const char **		defs	= 0;
	FILE *				fp		= 0;
	unsigned char		conv_str[128];
	char				line[128];
	char				squeezed_line[128];
	char				str_buf[128];
	char				trm_filename[MAX_FILELEN];
	char				trm_pathname[MAX_PATHLEN];
	char *				bptr;
	char *				cptr;
	char *				t1;
	char *				t2;
	char *				tp1;
	char *				tp2;
	const XLATE_LIST *	xp;
	int					in_qstr;
	int					have_eq;
	int					ix1;
	int					ix2;
	int					trm_debug;
	int					key;
	int					i;
	int					j;
	int					rc;

	/*--------------------------------------------------------------------
	 * check for def list pointer if first-time call
	 */
	if (term_list == 0)
	{
		defs = td->defs;

		/*----------------------------------------------------------------
		 * check if empty list
		 */
		if (defs != 0 && *defs == 0)
			defs = 0;
	}

	/*--------------------------------------------------------------------
	 * get name of term file to read
	 */
	for (i=0; term[i]; i++)
		trm_filename[i] = tolower(term[i]);
	trm_filename[i] = 0;
	strcat(trm_filename, ".trm");

	/*--------------------------------------------------------------------
	 * now try to open it
	 */
	rc = tcap_find_path(trm_filename, srch_path, trm_pathname);
	if (rc && defs == 0)
	{
		if (flag)
		{
			tcap_errmsg_add_fmt(td->errs, 
				TCAP_ERR_NOTERMFILE, TCAP_ERR_L_FATAL,
				"No term file found for term \"%s\"", term);
		}

		return (-1);
	}

	fp = fopen(trm_pathname, "r");
	if (fp == 0 && defs == 0)
	{
		if (flag)
		{
			tcap_errmsg_add_fmt(td->errs, 
				TCAP_ERR_CANTOPEN, TCAP_ERR_L_FATAL,
				"Cannot open term file \"%s\"", trm_pathname);
		}

		return (-1);
	}

	/*--------------------------------------------------------------------
	 * now process it
	 */
	rc = 0;
	trm_debug = 0;

	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * read in next line
		 */
		if (fp != 0)
		{
			if (fgets(line, sizeof(line), fp) == 0)
				break;
		}
		else
		{
			if (*defs == 0)
				break;

			strcpy(line, *defs++);
		}

		/*----------------------------------------------------------------
		 * if debug > 1 print out term-file line
		 */
		if (trm_debug > 1)
		{
			fputs(line, stderr);
			if (strchr(line, '\n') == 0)
				fputc('\n', stderr);
		}

		/*----------------------------------------------------------------
		 * remove any white space, hyphens, and underscores from line
		 * also stop at a * or a # if not in a quoted string
		 */
		in_qstr = FALSE;
		for (bptr=squeezed_line, cptr=line; *cptr; cptr++)
		{
			/*------------------------------------------------------------
			 * stop at EOL or comment
			 */
			if (! in_qstr)
			{
				if (*cptr == '\n' ||
				    *cptr == '*'  ||
				    *cptr == '#'  ||
				    *cptr == ';')
				{
					break;
				}
			}

			/*------------------------------------------------------------
			 * ignore white-space, '-', & '_'
			 */
			if (! in_qstr && (isspace(*cptr) || *cptr=='-' || *cptr=='_'))
				continue;

			*bptr++ = *cptr;
			if (*cptr == '"')
				in_qstr = ! in_qstr;
		}
		*bptr = 0;

		/*----------------------------------------------------------------
		 * check for comments or null lines
		 */
		if (*squeezed_line == 0)
			continue;

		/*----------------------------------------------------------------
		 * get left & right parts of line
		 */
		cptr = squeezed_line;
		t1 = cptr;
		have_eq = FALSE;
		while (*cptr)
		{
			if (*cptr == '=')
			{
				have_eq = TRUE;
				*cptr++ = 0;
				break;
			}

			*cptr = tolower(*cptr);
			cptr++;
		}

		t2 = cptr;

		/*----------------------------------------------------------------
		 * check if we found an equal sign
		 */
		if (! have_eq)
		{
			tcap_print_str(t1, str_buf);
			rc = 1;
			tcap_errmsg_add_fmt(td->errs, 
				TCAP_ERR_INV_SYNTAX, TCAP_ERR_L_FATAL,
				"Invalid format for line \"%s\"", str_buf);
			continue;
		}

		/*----------------------------------------------------------------
		 * look for debug entry
		 */
		if (tcap_trmcmp(t1, tcap_keyword_dbg) == 0)
		{
			trm_debug = tcap_tc_bool_val(t2);
			continue;
		}

		/*----------------------------------------------------------------
		 * look for dbtype entry
		 */
		if (tcap_trmcmp(t1, tcap_keyword_dbtype) == 0)
		{
			ix1 = tcap_db_name_to_val(t2);
			if (ix1 >= 0)
			{
				td->db_type = ix1;
			}
			else
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid database type \"%s\"", t2);
				rc = 1;
				continue;
			}
		}

		/*----------------------------------------------------------------
		 * look for term=name entry
		 */
		if (tcap_trmcmp(t1, tcap_keyword_term) == 0)
		{
			TERM_LIST *	tl;
			char *		ts;

			/*------------------------------------------------------------
			 * remove extra quotes if present
			 */
			if (*t2 == '"')
			{
				t2++;
				tp2 = t2 + strlen(t2)-1;
				if (*tp2 == '"')
					*tp2 = 0;
			}

			/*------------------------------------------------------------
			 * add this entry to the list
			 */
			ts = tcap_trm_dup_str(term);
			if (ts == 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_NOMEM, TCAP_ERR_L_FATAL,
					"No memory for term file reference for \"%s\"", t2);
				rc = 1;
				continue;
			}

			tl = (TERM_LIST *)MALLOC(sizeof(*tl));
			if (tl == 0)
			{
				FREE(ts);
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_NOMEM, TCAP_ERR_L_FATAL,
					"No memory for term file reference for \"%s\"", t2);
				rc = 1;
				continue;
			}

			tl->next  = term_list;
			tl->term  = ts;
			term_list = tl;

			/*------------------------------------------------------------
			 * check for cyclic references
			 */
			for (tl=term_list; tl; tl=tl->next)
			{
				if (tcap_trmcmp(t2, tl->term) == 0)
				{
					tcap_errmsg_add_fmt(td->errs, 
						TCAP_ERR_CYCLIC, TCAP_ERR_L_FATAL,
						"Cyclic term reference to \"%s\" in \"%s\" definition",
						t2, term);
					rc = 1;
					break;
				}
			}

			/*------------------------------------------------------------
			 * parse the referenced term file if not cyclic
			 */
			if (tl == 0)
			{
				tcap_db_load_term(tcap_db_get_type(), tp, t2);

				if (tcap_process_trm_file(tp, t2, srch_path, TRUE, term_list))
					rc = 1;
			}

			/*------------------------------------------------------------
			 * free the list entry
			 */
			tl = term_list->next;
			FREE(term_list->term);
			FREE(term_list);
			term_list = tl;

			continue;
		}

		/*----------------------------------------------------------------
		 * look for key entry
		 */
		key = -1;
	    ix1 = tcap_get_key_value(t1);
	    if (ix1 >= 0)
		{
			/*------------------------------------------------------------
			 * keyname = "xxxxx"
			 */
			key = ix1;
		}
		else if (tcap_trmcmp(t1, tcap_keyword_key) == 0)
		{
			tp1 = strchr(t2, ':');
			if (tp1 == 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid key definition \"%s\"", t2);
				rc = 1;
				continue;
			}
			*tp1++ = 0;

			t1 = t2;
			t2 = tp1;

			i = tcap_trans_str(t1, conv_str, sizeof(conv_str));

			if (i > 0)
			{
				key = 0;
				for (j=0; j<i; j++)
					key = (key * 256) + conv_str[j];
			}
		}

		if (key >= 0)
		{
			if (*t2 == '"')
			{
				if (t2[1] == '"')
				{
					/*----------------------------------------------------
					 * key-name = ""
					 *
					 * cancel any previous definitions
					 */
					tcap_kf_delkey(td, key);
				}
				else
				{
					/*----------------------------------------------------
					 * key-name = "xxxxxxxx"
					 *
					 * add definition to KFUNCTION list
					 */
					tcap_trans_term_str(t2, conv_str, sizeof(conv_str));
					tcap_kf_addkey(td, (char *)conv_str, key);
				}
			}
			else
			{
				/*--------------------------------------------------------
				 * key_name = other_key_name
				 */
	    		ix2 = tcap_get_key_value(t2);
				if (ix2 >= 0)
				{
					KFUNC *	k;

					/*----------------------------------------------------
					 * replace all occurrences of old-key with new-key
					 */
					for (k=td->kfuncs; k; k=k->kf_next)
					{
						if (k->kf_code == ix2)
							k->kf_code = key;
					}
				}
				else
				{
					tcap_errmsg_add_fmt(td->errs, 
						TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
						"Key name \"%s\" not found", t2);
					rc = 1;
					continue;
				}
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * look for termcap table entry
		 */
		{
			const TC_DATA *tc = tcap_tc_find_entry(t1);
			if (tc != 0)
			{
				if (tcap_tc_set_entry(td, tc, t2))
				{
					tcap_errmsg_add_fmt(td->errs, 
						TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
						"Invalid termcap name \"%s\"", t2);
					rc = 1;
				}
				continue;
			}
		}

		/*----------------------------------------------------------------
		 * look for color entry
		 */
		ix1 = tcap_get_fg_num_by_name(t1);
		ix2 = tcap_get_bg_num_by_name(t1);

		if (ix1 >= 0 || ix2 >= 0)
		{
			char **clrp;

			if (ix1 >= 0)
			{
				clrp = &td->strs.clr_fg[ix1];
				if (ix1+1 > td->ints.max_fg)
					td->ints.max_fg = ix1+1;
			}
			else
			{
				clrp = &td->strs.clr_bg[ix2];
				if (ix2+1 > td->ints.max_bg)
					td->ints.max_bg = ix2+1;
			}

			if (*clrp)
			{
				FREE(*clrp);
				*clrp = (char *)NULL;
			}

			if (*t2)
			{
				tp1 = 0;

	    		if (*t2 == '"')
				{
					tcap_trans_term_str(t2, conv_str, sizeof(conv_str));
					tp1 = (char *)conv_str;
	        	}
				else
				{
					/*----------------------------------------------------
					 *	Look for color = fg-name
					 *        or color = bg-name
					 */
					ix2 = tcap_get_fg_num_by_name(t2);
					if (ix2 >= 0)
					{
						tp1 = td->strs.clr_fg[ix2];
					}
					else
					{
						ix2 = tcap_get_bg_num_by_name(t2);
						if (ix2 >= 0)
						{
							tp1 = td->strs.clr_bg[ix2];
						}
					}

					if (tp1 == 0)
					{
						tcap_errmsg_add_fmt(td->errs, 
							TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
							"Invalid color name \"%s\"", t2);
						rc = 1;
					}
				}

				if (tp1)
				{
					tp2 = tcap_trm_dup_str(tp1);
					if (tp2)
					{
						*clrp = tp2;
					}
					else
					{
						tcap_errmsg_add_fmt(td->errs, 
							TCAP_ERR_NOMEM, TCAP_ERR_L_FATAL,
							"No memory for color entry \"%s\"",
							t1);
						rc = 1;
					}
				}
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * look for color fg map entry
		 */
		if (tcap_trmcmp(t1, tcap_keyword_map_fg) == 0)
		{
			tp1 = strchr(t2, ':');
			if (tp1 == 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid color fg map entry \"%s\"", t2);
				rc = 1;
				continue;
			}
			*tp1++ = 0;

			t1 = t2;
			t2 = tp1;

			ix1 = tcap_get_fg_num_by_name(t1);
			if (ix1 < 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid color fg map name \"%s\"", t1);
				rc = 1;
			}
			else
			{
				if (isdigit(*t2))
				{
					ix2 = atoi(t2) & 0x0f;
				}
				else
				{
					ix2 = tcap_get_fg_num_by_name(t2);
					if (ix2 < 0)
					{
						tcap_errmsg_add_fmt(td->errs, 
							TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
							"Invalid color fg map name \"%s\"",
							t2);
						rc = 1;
					}
				}

				if (ix2 >= 0)
					td->ints.color_fg_map[ix1] = ix2;
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * look for color bg map entry
		 */
		if (tcap_trmcmp(t1, tcap_keyword_map_bg) == 0)
		{
			tp1 = strchr(t2, ':');
			if (tp1 == 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid color bg map entry \"%s\"", t2);
				rc = 1;
				continue;
			}
			*tp1++ = 0;

			t1 = t2;
			t2 = tp1;

			ix1 = tcap_get_fg_num_by_name(t1);
			if (ix1 < 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid color bg map name \"%s\"", t1);
				rc = 1;
			}
			else
			{
				if (isdigit(*t2))
				{
					ix2 = atoi(t2) & 0x0f;
				}
				else
				{
					ix2 = tcap_get_fg_num_by_name(t2);
					if (ix2 < 0)
					{
						tcap_errmsg_add_fmt(td->errs, 
							TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
							"Invalid color bg map name \"%s\"",
							t2);
						rc = 1;
					}
				}

				if (ix2 >= 0)
					td->ints.color_bg_map[ix1] = ix2;
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * look for color map entry
		 */
		if (tcap_trmcmp(t1, tcap_keyword_map) == 0)
		{
			tp1 = strchr(t2, ':');
			if (tp1 == 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid color map entry \"%s\"", t2);
				rc = 1;
				continue;
			}
			*tp1++ = 0;

			t1 = t2;
			t2 = tp1;

			ix1 = tcap_get_fg_num_by_name(t1);
			if (ix1 < 0)
			{
				tcap_errmsg_add_fmt(td->errs, 
					TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
					"Invalid color map name \"%s\"", t1);
				rc = 1;
			}
			else
			{
				if (isdigit(*t2))
				{
					ix2 = atoi(t2) & 0x0f;
				}
				else
				{
					ix2 = tcap_get_fg_num_by_name(t2);
					if (ix2 < 0)
					{
						tcap_errmsg_add_fmt(td->errs, 
							TCAP_ERR_INV_DATA, TCAP_ERR_L_FATAL,
							"Invalid color map name \"%s\"", t2);
						rc = 1;
					}
				}

				if (ix2 >= 0)
				{
					td->ints.color_fg_map[ix1] = ix2;
					td->ints.color_bg_map[ix1] = ix2;
				}
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * look for translate table entry
		 */
		for (xp=tcap_xlate_list; xp->tbl_name; xp++)
		{
			if (tcap_trmcmp(t1, xp->tbl_name) == 0)
				break;
		}

		if (xp->tbl_name)
		{
			unsigned char *		tbl_ptr;
			unsigned char *		fnt_ptr;

			switch (xp->font_type)
			{
			case FONT_TBL_INP:
				tbl_ptr = td->inp_tbl;
				fnt_ptr = 0;
				break;

			case FONT_TBL_REG:
				tbl_ptr = td->reg_tbl;
				fnt_ptr = td->reg_font_tbl;
				break;

			case FONT_TBL_ALT:
				tbl_ptr = td->alt_tbl;
				fnt_ptr = td->alt_font_tbl;
				break;
			}

			ix2 = tcap_trans_str(t2, conv_str, sizeof(conv_str));
			ix2 &= ~1;			/* force to even value */

			for (i=0; i<ix2; i+=2)
			{
				j = conv_str[i];
				tbl_ptr[j] = conv_str[i+1];

				if (fnt_ptr)
					fnt_ptr[j] = xp->font_no;
			}
			continue;
		}

		/*----------------------------------------------------------------
		 * Ignore unrecognizable entries unless debug is set TRUE,
		 * since we may be reading someone else's table.
		 */
		if (trm_debug)
		{
			tcap_print_str(line, str_buf);
			tcap_errmsg_add_fmt(td->errs, 
				TCAP_ERR_UNKNOWN, TCAP_ERR_L_WARNING,
				"Unrecognized termfile entry \"%s\"", str_buf);
			rc = 1;
		}
   	}

	if (fp != 0)
    	fclose(fp);

	return (rc);
}
