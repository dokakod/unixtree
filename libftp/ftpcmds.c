/*------------------------------------------------------------------------
 * misc FTP commands
 */
#include "ftpcommon.h"

/*------------------------------------------------------------------------
 * ftp_set_cwd() - set CWD on a remote machine
 */
int ftp_set_cwd (FTP_NODE *ftp_ptr, const char *dir_name)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *p;

	p = fn_convert_dirname(dir_name, 0, ftp_ptr->server_type, path_name);

	retn = ftp_cmd(ftp_ptr, "CWD %s", p);
	if (retn == ERROR && ftp_ptr->code == 500)
	{
		/* CWD command not recognized, try XCWD */
		retn = ftp_cmd(ftp_ptr, "XCWD %s", p);
	}

	if (retn == COMPLETE)
	{
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_get_cwd() - get CWD on a remote machine
 */
int ftp_get_cwd (FTP_NODE *ftp_ptr, char *dir_name)
{
	int retn;
	char *p;
	char dir[MAX_PATHLEN];
	char node[MAX_FILELEN];
	char *dp = dir;

	retn = ftp_cmd(ftp_ptr, "PWD");

	if (retn == COMPLETE)
	{
		for (p=ftp_ptr->reply_string; *p && *p != '"'; p++)
			;
		for (p++; *p && *p != '"'; p++)
			*dp++ = *p;
		*dp = 0;

		x_fn_nodename(ftp_ptr->server_type, dir, node);
		fn_convert_dirname(dir, ftp_ptr->server_type, FN_SYSTEM, dir_name);

		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_make_dir() - make a directory on a remote machine
 */
int ftp_make_dir (FTP_NODE *ftp_ptr, const char *dir_name)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *p;

	p = fn_convert_dirname(dir_name, 0, ftp_ptr->server_type, path_name);

	retn = ftp_cmd(ftp_ptr, "MKD %s", p);
	if (retn == ERROR && ftp_ptr->code == 500)
	{
		/* MKD command not recognized, try XMKD */
		retn = ftp_cmd(ftp_ptr, "XMKD %s", p);
	}

	if (retn == COMPLETE)
	{
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_delete_dir() - delete a directory on a remote machine
 */
int ftp_delete_dir (FTP_NODE *ftp_ptr, const char *dir_name)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *p;

	p = fn_convert_dirname(dir_name, 0, ftp_ptr->server_type, path_name);

	retn = ftp_cmd(ftp_ptr, "RMD %s", p);
	if (retn == ERROR && ftp_ptr->code == 500)
	{
		/* RMD command not recognized, try XRMD */
		retn = ftp_cmd(ftp_ptr, "XRMD %s", p);
	}

	if (retn == COMPLETE)
	{
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_log_dir() - log a node on a remote machine
 */
int ftp_log_dir (FTP_NODE *ftp_ptr, const char *dir_name,
	int (*logrtn)(FTP_NODE *f, FTP_STAT *s, void *data), void *data)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *d;

	d = fn_convert_dirname(dir_name, 0, ftp_ptr->server_type, path_name);

	if (ftp_ptr->server_type == MSDOS_SERVER)
	{
		retn = ftp_set_cwd(ftp_ptr, dir_name);
		if (retn != 0)
		{
			return (retn);
		}
	}

	retn = ftp_recvrequest(ftp_ptr,
			(ftp_ptr->server_type == UNIX_SERVER ? "NLST -ali" : "LIST"),
			ftp_ptr->tempname, d);

	if (retn == 0)
	{
		retn = ftp_parse_directory_list(ftp_ptr, dir_name, logrtn, data);
	}

	os_file_delete(ftp_ptr->tempname);

	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_list_file() - get list of files in a directory on a remote machine
 */
int ftp_list_file (FTP_NODE *ftp_ptr, const char *dir_name,
	const char *file_name,
	int (*logrtn)(FTP_NODE *f, FTP_STAT *s, void *data), void *data)
{
	int retn;

	retn = ftp_set_cwd(ftp_ptr, dir_name);
	if (retn != 0)
	{
		return (retn);
	}

	retn = ftp_recvrequest(ftp_ptr,
			(ftp_ptr->server_type == UNIX_SERVER ? "NLST -aldi" : "LIST"),
			ftp_ptr->tempname, file_name);

	if (retn == 0)
	{
		retn = ftp_parse_directory_list(ftp_ptr, dir_name, logrtn, data);
	}

	os_file_delete(ftp_ptr->tempname);

	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_copy_file_to() - copy a file to a remote machine
 */
int ftp_copy_file_to (FTP_NODE *ftp_ptr, const char *local, const char *remote)
{
	int retn;
	struct stat st;
	register int c;
	register int d;
	FILE *fin;
	int dout;
	char buf[BUFSIZ];
	char obuf[BUFSIZ * 2];
	char *bufp;
	char *r;
	char path_name[MAX_PATHLEN];

	r = fn_convert_filename(remote, 0, ftp_ptr->server_type, path_name);

	/* if local not a regular file ... */
	if (os_stat(local, &st) < 0 || !S_ISREG(st.st_mode))
	{
		ftp_ptr->code = -1;
		return (-1);
	}

	/* open local file */
	fin = fopen(local, "rb");
	if (fin == (FILE *)NULL)
	{
		ftp_ptr->code = -1;
		return (-1);
	}

	/* start a listen on the data channel before we send the command */
	retn = ftp_initconn(ftp_ptr);
	if (retn)
	{
		ftp_ptr->code = -1;
		fclose(fin);
		return ((retn == -2) ? -2 : -1);
	}

	/* send STOR to set up server to accept data and store in remote */
	retn = ftp_cmd(ftp_ptr, "STOR %s", r);
	if (retn != PRELIM)
	{
		fclose(fin);
		return ((retn == -2) ? -2 : -1);
	}

	/* set up data connection */
	dout = ftp_dataconn(ftp_ptr);
	if (dout <= 0)
		goto abort;

	switch (ftp_ptr->curtype)
	{
		case TYPE_I:
		case TYPE_L:
			errno = d = 0;
			while ((c = fread(buf, 1, sizeof(buf), fin)) > 0)
			{
				for (bufp = buf; c > 0; c -= d, bufp += d)
				{
					if ((d = send(dout, bufp, c, 0)) <= 0)
						break;
				}
			}
			break;

		case TYPE_A:
			while ((c = fread(buf, 1, sizeof(buf), fin)) > 0)
			{
				int i;

				bufp = obuf;
				d = 0;
				for (i=0; i<c; i++)
				{
					if (buf[i] == '\n')
						obuf[d++] = '\r';
					obuf[d++] = buf[i];
				}
				c = d;

				errno = d = 0;
				for (bufp = obuf; c > 0; c -= d, bufp += d)
				{
					if ((d = send(dout, bufp, c, 0)) <= 0)
						break;
				}
			}
			break;
	}

	fclose(fin);
	closesocket(dout);

	retn = ftp_getreply(ftp_ptr, 0);
	if (retn == -2)
	{
		return (-2);
	}
	return (0);

abort:
	if (!ftp_ptr->cpend)
	{
		ftp_ptr->code = -1;
		return (-1);
	}
	if (ftp_ptr->data > 0)
	{
		closesocket(ftp_ptr->data);
		ftp_ptr->data = 0;
	}
	if (dout != 0)
	{
		closesocket(dout);
	}
	retn = ftp_getreply(ftp_ptr, 0);
	if (retn != -2)
	{
		retn = -1;
	}
	ftp_ptr->code = -1;
	fclose(fin);
	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_copy_file_from() - copy a file from a remote machine
 */
int ftp_copy_file_from (FTP_NODE *ftp_ptr,
	const char *local, const char *remote)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *r;

	r = fn_convert_filename(remote, 0, ftp_ptr->server_type, path_name);

	retn = ftp_recvrequest(ftp_ptr, "RETR", local, r);

	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_move_file_to() - move a file to a remote machine
 */
int ftp_move_file_to (FTP_NODE *ftp_ptr, const char *local, const char *remote)
{
	int retn;

	retn = ftp_copy_file_to(ftp_ptr, local, remote);

	if (retn == 0)
	{
		retn = os_file_delete(local);
	}
	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_move_file_from() - move a file from a remote machine
 */
int ftp_move_file_from (FTP_NODE *ftp_ptr,
	const char *local, const char *remote)
{
	int retn;

	retn = ftp_copy_file_from(ftp_ptr, remote, local);

	if (retn == 0)
	{
		retn = ftp_delete_file(ftp_ptr, remote);
	}
	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_delete_file() - delete a file on a remote machine
 */
int ftp_delete_file (FTP_NODE *ftp_ptr, const char *filename)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *f;

	f = fn_convert_filename(filename, 0, ftp_ptr->server_type, path_name);

	retn = ftp_cmd(ftp_ptr, "DELE %s", f);

	if (retn == COMPLETE)
	{
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_chmod_file() - change attributes of a file on a remote machine
 */
int ftp_chmod_file (FTP_NODE *ftp_ptr, const char *filename, int mode)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *f;

	f = fn_convert_filename(filename, 0, ftp_ptr->server_type, path_name);

	retn = ftp_cmd(ftp_ptr, "SITE CHMOD 0%o %s", mode, f);

	if (retn == COMPLETE)
	{
		return (0);
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_rename_file() - rename a file on a remote machine
 */
int ftp_rename_file (FTP_NODE *ftp_ptr, const char *from_name,
	const char *to_name)
{
	char path_name[MAX_PATHLEN];
	int retn;
	char *fr;
	char *to;

	fr = fn_convert_filename(from_name, 0, ftp_ptr->server_type, path_name);

	retn = ftp_cmd(ftp_ptr, "RNFR %s", fr);

	if (retn == CONTINUE)
	{
		to = fn_convert_filename(to_name, 0, ftp_ptr->server_type, path_name);
		retn = ftp_cmd(ftp_ptr, "RNTO %s", to);
		if (retn == COMPLETE)
		{
			return (0);
		}
	}
	return ((retn == -2) ? -2 : -1);
}

/*------------------------------------------------------------------------
 * ftp_copy_file_between() - copy a file from a remote mach to a remote mach
 */
int ftp_copy_file_between (FTP_NODE *ftp_src, FTP_NODE *ftp_dest,
	const char *srcfile, const char *destfile)
{
	int retn;

	/* make copy */
	retn = ftp_copy_file_from(ftp_src, srcfile, ftp_src->tempname);

	if (retn == 0)
	{
		retn = ftp_copy_file_to(ftp_dest, ftp_src->tempname, destfile);
	}
	os_file_delete(ftp_src->tempname);

	return (retn);
}

/*------------------------------------------------------------------------
 * ftp_move_file_between() - move a file from a remote mach to a remote mach
 */
int ftp_move_file_between (FTP_NODE *ftp_src, FTP_NODE *ftp_dest,
	const char *srcfile, const char *destfile)
{
	int retn;

	retn = ftp_copy_file_between(ftp_src, ftp_dest, srcfile, destfile);

	if (retn == 0)
	{
		retn = ftp_delete_file(ftp_src, srcfile);
	}
	return (retn);
}
