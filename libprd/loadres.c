/*------------------------------------------------------------------------
 * load all resource files
 */
#include "libprd.h"

static void add_reslist_entry (RES_FILE *rf)
{
	RES_INFO *	ri;

	ri = (RES_INFO *)MALLOC(sizeof(*ri));
	if (ri != 0)
	{
		strcpy(ri->disp_name, rf->hdr.name);
		strcpy(ri->file_name, rf->hdr.file);
		ri->res_ptr = rf;

		gbl(res_list) = bappend(gbl(res_list), BNEW(ri));
	}
}

static void resource_list_load (void)
{
	UTDIR *	utdir;
	int		rc;

	/*--------------------------------------------------------------------
	 * set current resource file to built-in
	 */
	gbl(cur_res) = &res_file;

	/*--------------------------------------------------------------------
	 * add built-in resource as first entry in resource list
	 */
	add_reslist_entry(&res_file);

	/*--------------------------------------------------------------------
	 * now add any resource files found in pgm directory
	 */
	utdir = os_dir_open(gbl(pgm_dir));
	if (utdir != 0)
	{
		while (TRUE)
		{
			char		filename[MAX_FILELEN];
			char		res_path[MAX_PATHLEN];
			RES_FILE *	rf;
			char *		ext;

			/*------------------------------------------------------------
			 * read in next dir entry
			 */
			rc = os_dir_read(utdir, filename);
			if (rc)
				break;

			/*------------------------------------------------------------
			 * check if it's a resource file
			 */
			ext = fn_ext(filename);
			if (ext == 0 || strccmp(ext, pgm_const(res_ext)) != 0)
				continue;

			/*------------------------------------------------------------
			 * create full path of resource file
			 */
			strcpy(res_path, gbl(pgm_dir));
			fn_append_filename_to_dir(res_path, filename);

			/*------------------------------------------------------------
			 * load this resource file
			 */
			rf = res_file_load(res_path, FALSE);
			if (rf == 0)
				continue;

			/*------------------------------------------------------------
			 * if load successful, add to resource lists
			 */
			add_reslist_entry(rf);
		}

		os_dir_close(utdir);
	}
}

static void resource_list_free (void)
{
	BLIST *b;

	for (b=gbl(res_list); b; b=bnext(b))
	{
		RES_INFO *ri = (RES_INFO *)bid(b);
		RES_FILE *rf = ri->res_ptr;

		res_file_free(rf);
	}

	gbl(res_list) = BSCRAP(gbl(res_list), TRUE);
}

static int resource_file_find (const char *lang)
{
	BLIST *	b;
	int		i;

	for (i=0, b=gbl(res_list); b; i++, b=bnext(b))
	{
		RES_INFO *ri = (RES_INFO *)bid(b);
		RES_FILE *rf = ri->res_ptr;

		if (strccmp(lang, rf->hdr.name) == 0 ||
		    strccmp(lang, rf->hdr.file) == 0)
		{
			return (i);
		}
	}

	return (-1);
}

void load_res (void)
{
	resource_list_load();
}

void free_res (void)
{
	resource_list_free();
	gbl(cur_res) = &res_file;
}

int select_lang_by_name (const char *lang)
{
	int num;

	num = resource_file_find(lang);
	if (num < 0)
		return (-1);

	return select_lang_by_num(num);
}

int select_lang_by_num (int num)
{
	RES_INFO *	ri;
	int			cur	= get_cur_lang();
	BLIST *		b;

	/*--------------------------------------------------------------------
	 * check if same as current language
	 */
	if (cur == num)
		return (0);

	/*--------------------------------------------------------------------
	 * find new language entry
	 */
	b = bnth(gbl(res_list), num);
	if (b == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * set new language pointer
	 */
	ri = (RES_INFO *)bid(b);
	gbl(cur_res) = ri->res_ptr;

	return (0);
}

int get_cur_lang (void)
{
	RES_INFO *	ri;
	int			cur;
	BLIST *		b;

	for (cur = 0, b = gbl(res_list); b; cur++, b = bnext(b))
	{
		ri = (RES_INFO *)bid(b);
		if (ri->res_ptr == gbl(cur_res))
			break;
	}

	return (cur);
}
