/*------------------------------------------------------------------------
 * process the "batch" cmd
 */
#include "libprd.h"

struct batch_info
{
	FILE *	batch_file;
	char	tplate[MAX_PATHLEN];
};
typedef struct batch_info BATCH_INFO;

static int do_batch (void *data)
{
	BATCH_INFO *bi = (BATCH_INFO *)data;
	char output_line[BUFSIZ];

	expand_cmd(bi->tplate, output_line);
	strcat(output_line, "\n");
	fputs(output_line, bi->batch_file);
	return (1);					/* for traverse() */
}

void batch (void)
{
	BATCH_INFO	batch_info;
	BATCH_INFO *bi = &batch_info;
	char	batch_filename[MAX_PATHLEN];
	char input_str[MAX_PATHLEN];
	int c;
	char *p, *q;

	if (check_tag_count())
		return;

	bang(msgs(m_batch_entfil));
	werase(gbl(win_commands));
	wmove(gbl(win_commands), 0, 0);
	p = msgs(m_batch_filnam);
	q = msgs(m_batch_tem);
	xaddstr(gbl(win_commands), p);
	wrefresh(gbl(win_commands));
	*input_str = 0;
	c = xgetstr(gbl(win_commands), input_str, XGL_BATCH_FILENAME, MAX_PATHLEN,
		0, XG_FILEPATH);
	if (c <= 0)
		return;

	fn_get_abs_path(gbl(scr_cur)->path_name, input_str, batch_filename);
	bang(msgs(m_batch_1234));
	wmove(gbl(win_commands), 1, display_len(p)-display_len(q));
	waddstr(gbl(win_commands), q);
	wrefresh(gbl(win_commands));
	*bi->tplate = 0;
	c = xgetstr(gbl(win_commands), bi->tplate, XGL_TEMPLATE, MAX_PATHLEN, 0,
		XG_STRING);
	if (c <= 0)
		return;

	bi->batch_file = fopen(batch_filename, "w");
	if (bi->batch_file == 0)
	{
		errsys(ER_COBTCH);
		return;
	}

	traverse(do_batch, bi);
	fclose(bi->batch_file);
	os_chmod(batch_filename, S_IRWXU | S_IRWXG | S_IRWXO);
	check_the_file(batch_filename);
}

void expand_cmd (const char *inp_str, char *out_str)
{
	TREE *parent;
	const char *p;
	char *o;

	o = out_str;
	*o = 0;
	for (p=inp_str; *p; p++)
	{
		if (*p == '$')
		{
			switch (*++p)
			{
			case '1':
					strcpy(o, gbl(scr_cur)->path_name);
					fn_append_filename_to_dir(o,
						FULLNAME(gbl(scr_cur)->cur_file));
					o = out_str+strlen(out_str);
					break;
			case '2':
					strcpy(o, gbl(scr_cur)->path_name);
					o = out_str+strlen(out_str);
					break;
			case '3':
					strcpy(o, FULLNAME(gbl(scr_cur)->cur_file));
					o = out_str+strlen(out_str);
					break;
			case '4':
					parent = (gbl(scr_cur)->cur_file)->dir->dir_tree;
					if (tparent(parent))
						parent = tparent(parent);
					dirtree_to_dirname(parent, o);
					o = out_str+strlen(out_str);
					break;
			case '$':
					*o++ = '$';
					*o   = 0;
					break;
			default:
					*o++ = '$';
					*o++ = *p;
					*o   = 0;
					break;
			}
		}
		else
		{
			*o++ = *p;
			*o = 0;
		}
	}
}
