/*------------------------------------------------------------------------
 * read/write a compiled term file
 *
 *	Format of a compiled term file is as follows:
 *		magic number		4 bytes
 *		size of char data	4 bytes
 *		all ints			4 bytes each
 *		all bools			4 bytes each
 *		all param ptrs		4 bytes each (0 if present, -1 if not)
 *		all string ptrs		4 bytes each (0 if present, -1 if not)
 *		xlate tbls			3 * 256 bytes
 *		font tbls			2 * 256 bytes
 *		num function keys	4 bytes
 *		function key list	4 bytes each (key code)
 *		char str data		(size of char data) bytes
 */
#include "tcapcommon.h"

#define TRC_MAGIC 0x05241992	/* magic number for file */

/*------------------------------------------------------------------------
 *  tcap_put_trc() -  write a compiled term file
 */
int tcap_put_trc (const TERMINAL *tp, const char *path)
{
	TCAP_DATA *	td;
	FILE *		fp;
	KFUNC *		k;
	char *		p;
	char **		cp;
	char **		pp;
	int *		ip;
	int *		bp;
	char *		char_ptr;
	int			i;
	int			char_size;
	int			num;
	int			nk;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || path == 0 || *path == 0)
		return (-1);

	td = tp->tcap;
	pp = (char **)&td->parms;
	cp = (char **)&td->strs;
	ip = (int   *)&td->ints;
	bp = (int   *)&td->bools;

	/*--------------------------------------------------------------------
	 * get size of all char strings
	 */
	char_size = 0;

	num = sizeof(td->parms) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		if (is_cmd_pres(pp[i]))
			char_size += strlen(pp[i]) + 1;
	}

	num = sizeof(td->strs) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		if (is_cmd_pres(cp[i]))
			char_size += strlen(cp[i]) + 1;
	}

	nk = 0;
	for (k=td->kfuncs; k; k=k->kf_next)
	{
		char_size += strlen(k->kf_str) + 1;
		nk++;
	}

	char_size = (char_size + 3) & ~3;

	/*--------------------------------------------------------------------
	 * malloc array and copy all strings
	 */
	char_ptr = (char *)MALLOC(char_size);
	if (char_ptr == 0)
		return (-1);
	memset(char_ptr, 0, char_size);

	p = char_ptr;

	num = sizeof(td->parms) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		if (is_cmd_pres(pp[i]))
		{
			strcpy(p, pp[i]);
			p += strlen(p) + 1;
		}
	}

	num = sizeof(td->strs) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		if (is_cmd_pres(cp[i]))
		{
			strcpy(p, cp[i]);
			p += strlen(p) + 1;
		}
	}

	for (k=td->kfuncs; k; k=k->kf_next)
	{
		strcpy(p, k->kf_str);
		p += strlen(p) + 1;
	}

	/*--------------------------------------------------------------------
	 * open file
	 */
	fp = fopen(path, "wb");
	if (fp == 0)
	{
		FREE(char_ptr);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * write all data
	 */
	tcap_put_4byte(fp, TRC_MAGIC);
	tcap_put_4byte(fp, char_size);
	tcap_put_4byte(fp, td->db_type);

	num = sizeof(td->ints) / sizeof(int);
	for (i=0; i<num; i++)
	{
		tcap_put_4byte(fp, ip[i]);
	}

	num = sizeof(td->bools) / sizeof(int);
	for (i=0; i<num; i++)
	{
		tcap_put_4byte(fp, bp[i]);
	}

	num = sizeof(td->parms) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		tcap_put_4byte(fp, is_cmd_pres(pp[i]) ? 0 : (unsigned int)-1);
	}

	num = sizeof(td->strs) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		tcap_put_4byte(fp, is_cmd_pres(cp[i]) ? 0 : (unsigned int)-1);
	}

	fwrite(td->inp_tbl,      sizeof(td->inp_tbl),      1, fp);
	fwrite(td->reg_tbl,      sizeof(td->reg_tbl),      1, fp);
	fwrite(td->alt_tbl,      sizeof(td->alt_tbl),      1, fp);

	fwrite(td->reg_font_tbl, sizeof(td->reg_font_tbl), 1, fp);
	fwrite(td->alt_font_tbl, sizeof(td->alt_font_tbl), 1, fp);

	tcap_put_4byte(fp, nk);
	for (k=td->kfuncs; k; k=k->kf_next)
	{
		tcap_put_4byte(fp, k->kf_code);
	}

	fwrite(char_ptr, char_size, 1, fp);

	/*--------------------------------------------------------------------
	 * close file & return
	 */
	fclose(fp);
	FREE(char_ptr);

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_get_trc() - read a compiled term file
 */
int tcap_get_trc (TERMINAL *tp, const char *path)
{
	TCAP_DATA *		td;
	FILE *			fp;
	unsigned int	magic;
	int				char_size;
	char *			char_ptr;
	char *			p;
	char **			pp;
	char **			cp;
	KFUNC *			kp;
	KFUNC *			np;
	int *			ip;
	int *			bp;
	int				num;
	int				i;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (tp == 0 || path == 0 || *path == 0)
		return (-1);

	td = tp->tcap;
	pp = (char **)&td->parms;
	cp = (char **)&td->strs;
	ip = (int   *)&td->ints;
	bp = (int   *)&td->bools;

	/*--------------------------------------------------------------------
	 * open file
	 */
	fp = fopen(path, "rb");
	if (fp == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * check magic number
	 */
	magic = tcap_get_4byte(fp);
	if (magic != TRC_MAGIC)
	{
		fclose(fp);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * get char table size & malloc it
	 */
	char_size = (int)tcap_get_4byte(fp);
	char_ptr = (char *)MALLOC(char_size);
	if (char_ptr == 0)
	{
		fclose(fp);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * read in struct
	 */
	td->db_type = (int)tcap_get_4byte(fp);

	num = sizeof(td->ints) / sizeof(int);
	for (i=0; i<num; i++)
		ip[i] = (int)tcap_get_4byte(fp);

	num = sizeof(td->bools) / sizeof(int);
	for (i=0; i<num; i++)
		bp[i] = (int)tcap_get_4byte(fp);

	num = sizeof(td->parms) / sizeof(char *);
	for (i=0; i<num; i++)
		pp[i] = (char *)tcap_get_4byte(fp);

	num = sizeof(td->strs) / sizeof(char *);
	for (i=0; i<num; i++)
		cp[i] = (char *)tcap_get_4byte(fp);

	fread(td->inp_tbl,      sizeof(td->inp_tbl),      1, fp);
	fread(td->reg_tbl,      sizeof(td->reg_tbl),      1, fp);
	fread(td->alt_tbl,      sizeof(td->alt_tbl),      1, fp);

	fread(td->reg_font_tbl, sizeof(td->reg_font_tbl), 1, fp);
	fread(td->alt_font_tbl, sizeof(td->alt_font_tbl), 1, fp);

	num = (int)tcap_get_4byte(fp);
	np = 0;
	for (i=0; i<num; i++)
	{
		kp = tcap_kf_new();
		if (kp == 0)
			break;

		kp->kf_code = (int)tcap_get_4byte(fp);

		if (np == 0)
		{
			td->kfuncs = kp;
		}
		else
		{
			kp->kf_prev = np;
			np->kf_next = kp;
		}

		np = kp;
	}

	fread(char_ptr, char_size, 1, fp);

	/*--------------------------------------------------------------------
	 * close file
	 */
	fclose(fp);

	/*--------------------------------------------------------------------
	 * convert all string pointers
	 */
	p = char_ptr;

	num = sizeof(td->parms) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		if (pp[i] != (char *)-1)
		{
			pp[i] = tcap_trm_dup_str(p);
			p += strlen(p) + 1;
		}
	}

	num = sizeof(td->strs) / sizeof(char *);
	for (i=0; i<num; i++)
	{
		if (cp[i] != (char *)-1)
		{
			cp[i] = tcap_trm_dup_str(p);
			p += strlen(p) + 1;
		}
	}

	/*--------------------------------------------------------------------
	 * copy all function-key strings
	 */
	for (kp=td->kfuncs; kp; kp=kp->kf_next)
	{
		kp->kf_str = tcap_trm_dup_str(p);
		p += strlen(p) + 1;
	}

	/*--------------------------------------------------------------------
	 * free the char array
	 */
	FREE(char_ptr);

	return (0);
}
