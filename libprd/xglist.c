/*------------------------------------------------------------------------
 * menu lists
 */
#include "libprd.h"

#define XG_SAVE_KEEP	'!'
#define XG_SAVE_NOKEEP	'-'

struct xg_list_tbl
{
	int				tbl_no;
	const char *	name;
};
typedef struct xg_list_tbl XG_LIST_TBL;

static const XG_LIST_TBL xg_tbl[] =
{
	{ XGL_CFG_CHR,			"[config-chars]"		},
	{ XGL_CFG_FILE,			"[config-filenames]"	},
	{ XGL_CFG_NUM,			"[config-numbers]"		},
	{ XGL_CFG_STR,			"[config-strings]"		},
	{ XGL_ARCH_NAME,		"[archive-names]"		},
	{ XGL_EXEC,				"[exec-programs]"		},
	{ XGL_EXT_REL_PATH,		"[archive-rel-paths]"	},
	{ XGL_GOTO_FILE,		"[goto-filenames]"		},
	{ XGL_GOTO_DIR,			"[goto-dirnames]"		},
	{ XGL_NODE_PATH,		"[saved-node-paths]"	},
	{ XGL_REL_BASE,			"[archive-rel-bases]"	},
	{ XGL_BATCH_FILENAME,	"[batch-filenames]"		},
	{ XGL_EDIT_FILE,		"[edit-filenames]"		},
	{ XGL_PRT_FILE,			"[print-filenames]"		},
	{ XGL_SRCH_STR,			"[search-strings]"		},
	{ XGL_TEMPLATE,			"[templates]"			},
	{ XGL_FILE_TO_COPY,		"[copy-filenames]"		},
	{ XGL_CMD,				"[exec-cmds]"			},
	{ XGL_DATE,				"[attr-dates]"			},
	{ XGL_DEST,				"[copy-dest-dirs]"		},
	{ XGL_DEVICE,			"[archive-devices]"		},
	{ XGL_FILESPEC,			"[filespecs]"			},
	{ XGL_GROUP,			"[attr-groups]"			},
	{ XGL_HOST,				"[ftp-hostnames]"		},
	{ XGL_INODE,			"[attr-inodes]"			},
	{ XGL_NEWNAME,			"[newdir-names]"		},
	{ XGL_NLINK,			"[attr-nlinks]"			},
	{ XGL_NODE,				"[node-paths]"			},
	{ XGL_OWNER,			"[attr-owners]"			},
	{ XGL_PATTERN,			"[patterns]"			},
	{ XGL_RDIR,				"[ftp-dirs]"			},
	{ XGL_RELNODE,			"[archive-rel-nodes]"	},
	{ XGL_RENAME,			"[rename-names]"		},
	{ XGL_SIZE,				"[attr-sizes]"			},
	{ XGL_USER,				"[ftp-users]"			},
	{ XGL_PRINT_FILE_NAME,	"[ftp-print-filenames]"	},
	{ XGL_SYM_NAMES,		"[sym-names]"			},
	{ XGL_TREESPEC,			"[treespec]"			},
	{ XGL_DIFFNAMES,		"[diff-files]"			},
	{ XGL_DIFFRPT,			"[diff-reports]"		},
	{ XGL_GET_PERMS,		"[get-perms]"			},
	{ XGL_SET_PERMS,		"[set-perms]"			},

	{ 0, 0 }
};

void xg_ent_free (XG_ENTRY *xe)
{
	FREE(xe->line);
	FREE(xe);
}

void xg_tbl_free (void)
{
	if (gbl(xgl_list) != 0)
	{
		const XG_LIST_TBL *t;

		for (t=xg_tbl; t->tbl_no; t++)
		{
			XG_LIST *	lp = xg_tbl_find(t->tbl_no);

			if (lp->num)
			{
				BLIST *b;

				for (b = lp->lines; b; b = bnext(b))
				{
					XG_ENTRY *xe = (XG_ENTRY *)bid(b);

					xg_ent_free(xe);
				}
				lp->lines = BSCRAP(lp->lines, FALSE);
				lp->num = 0;
			}
		}

		FREE(gbl(xgl_list));
		gbl(xgl_list) = 0;
	}
}

static XG_LIST * find_tbl (const char *str)
{
	const XG_LIST_TBL *t;

	for (t=xg_tbl; t->tbl_no; t++)
	{
		if (strccmp(str, t->name) == 0)
			return xg_tbl_find(t->tbl_no);
	}

	return (0);
}

static char * tbl_path (char *path)
{
	char filename[MAX_FILELEN];

	strcpy(filename, pgmi(m_pgm_program));
	fn_set_ext(filename, pgm_const(hst_ext));

	strcpy(path, gbl(pgm_home));
	fn_append_filename_to_dir(path, filename);
	fn_resolve_pathname(path);

	return (path);
}

void xg_tbl_load (void)
{
	char path[MAX_PATHLEN];
	FILE *fp;

	/*--------------------------------------------------------------------
	 * delete any current history list
	 */
	if (gbl(xgl_list) != 0)
		xg_tbl_free();

	/*--------------------------------------------------------------------
	 * allocate new list
	 */
	gbl(xgl_list) =
		(XG_LIST *)MALLOC(XGL_NUM_ENTRIES * sizeof(*gbl(xgl_list)));
	if (gbl(xgl_list) == 0)
		return;

	memset(gbl(xgl_list), 0,
		XGL_NUM_ENTRIES * sizeof(*gbl(xgl_list)));

	/*--------------------------------------------------------------------
	 * open the history file
	 */
	tbl_path(path);
	fp = fopen(path, "r");

	/*--------------------------------------------------------------------
	 * read the history file if found
	 */
	if (fp != 0)
	{
		XG_LIST *xg = 0;

		while (TRUE)
		{
			char line[MAX_PATHLEN];

			if (fgets(line, sizeof(line), fp) == 0)
				break;
			strip(line);

			if (*line == 0)
				continue;

			if (*line == '[')
			{
				xg = find_tbl(line);
			}
			else
			{
				if (xg != 0)
					xg_tbl_add(xg, line+1, *line == XG_SAVE_KEEP);
			}
		}

		fclose(fp);
	}
}

static void xt_tbl_save_entry (FILE *fp, XG_ENTRY *xe)
{
	fprintf(fp, "%c%s\n",
		xe->keep ? XG_SAVE_KEEP : XG_SAVE_NOKEEP,
		xe->line);
}

void xg_tbl_save (void)
{
	char path[MAX_PATHLEN];
	FILE *fp;

	/*--------------------------------------------------------------------
	 * bail if no history to save
	 */
	if (gbl(xgl_list) == 0)
		return;

	/*--------------------------------------------------------------------
	 * create home directory if non-existent
	 */
	if (make_home_dir())
		return;

	/*--------------------------------------------------------------------
	 * open history file
	 */
	tbl_path(path);
	fp = fopen(path, "w");

	/*--------------------------------------------------------------------
	 * save all history entries
	 */
	if (fp != 0)
	{
		const XG_LIST_TBL *t;

		for (t=xg_tbl; t->tbl_no; t++)
		{
			XG_LIST *	lp = xg_tbl_find(t->tbl_no);

			if (lp->num)
			{
				BLIST *b;

				fprintf(fp, "%s\n", t->name);

				if (opt(max_saved_entries) <= 0)
				{
					/*----------------------------------------------------
					 * save all entries in list
					 */
					for (b = lp->lines; b; b = bnext(b))
					{
						XG_ENTRY *xe = (XG_ENTRY *)bid(b);

						xt_tbl_save_entry(fp, xe);
					}
				}
				else
				{
					/*----------------------------------------------------
					 * save all keep entries & up to max_saved_entries
					 * non-keep entries
					 *
					 * We count all "non-keep" entries, and then save
					 * the last "max_saved_entries" of them.
					 */
					int nk = 0;
					int skip;

					for (b = lp->lines; b; b = bnext(b))
					{
						XG_ENTRY *xe = (XG_ENTRY *)bid(b);

						if (! xe->keep)
							nk++;
					}

					if (nk <= opt(max_saved_entries))
						skip = 0;
					else
						skip = (nk - opt(max_saved_entries));

					for (b = lp->lines; b; b = bnext(b))
					{
						XG_ENTRY *xe = (XG_ENTRY *)bid(b);
						int keep = xe->keep;

						if (! keep)
						{
							if (skip-- <= 0)
								keep = TRUE;
						}

						if (keep)
							xt_tbl_save_entry(fp, xe);
					}
				}
			}
		}

		fclose(fp);
	}

	os_chmod(path, 0600);
}

XG_LIST *xg_tbl_find (int tbl_no)
{
	if (gbl(xgl_list) == 0)
		return (0);

	if (tbl_no <= 0 || tbl_no > XGL_NUM_ENTRIES)
		return (0);

	return (gbl(xgl_list) + tbl_no - 1);
}

void xg_tbl_add (XG_LIST *xg, const char *string, int keep)
{
	XG_ENTRY *	xe;
	BLIST *		b;
	char *		s;

	/*--------------------------------------------------------------------
	 * bail if null or empty string
	 */
	if (xg == 0 || string == 0 || *string == 0)
		return;

	/*--------------------------------------------------------------------
	 * check if this is a duplicate of any entry
	 */
	if (xg->num)
	{
		for (b=xg->lines; b; b=bnext(b))
		{
			xe = (XG_ENTRY *)bid(b);

			if (strcmp(string, xe->line) == 0)
				break;
		}

		if (b != 0)
		{
			if (keep)
				xe->keep = keep;

			xg->lines = bremove(xg->lines, b);
			xg->lines = bappend(xg->lines, b);
			return;
		}
	}

	/*--------------------------------------------------------------------
	 * get copy of string
	 */
	s = STRDUP(string);
	if (s == 0)
	{
		return;
	}

	/*--------------------------------------------------------------------
	 * create entry
	 */
	xe = (XG_ENTRY *)MALLOC(sizeof(*xe));
	if (xe == 0)
	{
		FREE(s);
		return;
	}

	xe->keep = keep;
	xe->line = s;

	/*--------------------------------------------------------------------
	 * create list entry
	 */
	b = BNEW(xe);
	if (b == 0)
	{
		FREE(xe);
		FREE(s);
		return;
	}

	/*--------------------------------------------------------------------
	 * add to list
	 */
	xg->lines = bappend(xg->lines, b);
	xg->num++;
}

void xg_tbl_mark (XG_LIST *xg, XG_ENTRY *xe)
{
	if (xg != 0 && xe != 0)
	{
		BLIST *be = bfind(xg->lines, xe);

		xg->lines = bremove(xg->lines, be);
		xg->lines = bappend(xg->lines, be);
	}
}
