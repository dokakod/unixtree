/*------------------------------------------------------------------------
 * routines to process termcap entries
 */
#include "tcapcommon.h"

/*------------------------------------------------------------------------
 * list of termcap directories to check
 */
static const char * tcap_tc_dirs[] =
{
	"/etc/termcap",
	"/usr/share/termcap",
	"/usr/share/misc/termcap",
	0
};

/*------------------------------------------------------------------------
 * strip all white-space from the end of a line
 */
static int tcap_strip (char *line)
{
	char *s;
	char *p = line;

	for (s=line; *s; s++)
	{
		if (! isspace(*s))
			p = s+1;
	}
	*p = 0;

	return (p - line);
}

/*------------------------------------------------------------------------
 * read in a line, skipping comment lines and processing mult lines
 */
static int tcap_read_line (FILE *fp, char *buf)
{
	char	line[BUFSIZ];
	char *	obuf	= buf;
	char *	lp;
	int		l;

	/*--------------------------------------------------------------------
	 * read in lines until a non-continued line is read, skipping comments
	 */
	*buf = 0;
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * read in next line
		 */
		if (fgets(line, sizeof(line), fp) == 0)
			return (-1);

		/*----------------------------------------------------------------
		 * look for 1st non-blank character
		 */
		for (lp=line; *lp; lp++)
		{
			if (! (isspace(*lp) || *lp == ':') )
				break;
		}

		/*----------------------------------------------------------------
		 * now strip all trailing white-space
		 */
		l = tcap_strip(lp);

		/*----------------------------------------------------------------
		 * empty line is end
		 */
		if (*lp == 0)
			break;

		/*----------------------------------------------------------------
		 * skip comment lines
		 */
		if (*lp == '#')
			continue;

		/*----------------------------------------------------------------
		 * check if valid entry
		 */
		if (buf == obuf && *lp == ':')
			continue;

		/*----------------------------------------------------------------
		 * add to line buffer
		 */
		strcpy(buf, lp);

		/*----------------------------------------------------------------
		 * break if not continued line
		 */
		if (buf[l-1] != '\\')
			break;

		buf[l-1] = 0;
		buf += l-1;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * check if line contains term name
 */
static int tcap_check_line (const char *buf, const char *term)
{
	char			test[128];
	char *			t;
	const char *	b;

	/*--------------------------------------------------------------------
	 * check first part of line
	 *
	 * This will be in form "xxx|yyy|zzz|description:..."
	 */
	for (b=buf; *b; b++)
	{
		int i;

		/*----------------------------------------------------------------
		 * collect next part into test buffer
		 */
		for (i=0, t=test; *b && i < (int)sizeof(test)-1; i++, b++)
		{
			if (*b == '|' || *b == ':')
				break;

			*t++ = *b;
		}
		*t = 0;

		/*----------------------------------------------------------------
		 * compare against name
		 */
		if (strcmp(test, term) == 0)
			return (0);

		/*----------------------------------------------------------------
		 * if delimiter is a ':', we're done
		 */
		if (*b == ':')
			break;
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * find an entry in a buffer
 */
static const char * tcap_find_name_orig (const char *buf, const char *name)
{
	/*--------------------------------------------------------------------
	 * loop through buffer looking for an entry
	 *
	 * Note: we have insured that the buffer ALWAYS starts & ends with a ':'
	 */
	while (TRUE)
	{
		const char *	p;
		const char *	n;

		/*----------------------------------------------------------------
		 * get ptr to next field
		 */
		p = strchr(buf, ':');
		if (p == 0)
			break;

		/*----------------------------------------------------------------
		 * if last delimiter, done
		 */
		if (*++p == 0)
			break;

		/*----------------------------------------------------------------
		 * compare for length of name
		 */
		for (n=name; *n; n++)
		{
			if (*p++ != *n)
				break;
		}

		/*----------------------------------------------------------------
		 * if match, return pointer to string after name
		 */
		if (*n == 0)
			return (p);

		buf = p;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * find an entry in a buffer
 */
static const char * tcap_find_name (const char *buf, const char *name)
{
	/*--------------------------------------------------------------------
	 * loop through buffer looking for an entry
	 *
	 * Note: we have insured that the buffer ALWAYS starts with a NULL
	 * and ends with a NULL-NULL.
	 */
	while (TRUE)
	{
		const char *	p;
		const char *	n;

		/*----------------------------------------------------------------
		 * get ptr to next field
		 */
		p = strchr(buf, 0);
		if (p == 0)
			break;

		/*----------------------------------------------------------------
		 * if last delimiter, done
		 */
		if (*++p == 0)
			break;

		/*----------------------------------------------------------------
		 * compare for length of name
		 */
		for (n=name; *n; n++)
		{
			if (*p++ != *n)
				break;
		}

		/*----------------------------------------------------------------
		 * if match, return pointer to string after name
		 */
		if (*n == 0)
			return (p);

		buf = p;
	}

	return (0);
}

/*------------------------------------------------------------------------
 * check if line contains tc=xxx entry
 */
static int tcap_check_tc (const char *buf, char *term)
{
	const char *	b;
	char *			t;

	/*--------------------------------------------------------------------
	 * check if tc= entry is present
	 */
	b = tcap_find_name_orig(buf, "tc");
	if (b == 0)
		return (-1);

	if (*b != '=')
		return (-1);

	/*--------------------------------------------------------------------
	 * collect term name after entry
	 */
	b++;
	for (t=term; *b; b++)
	{
		if (*b == ':')
			break;

		*t++ = *b;
	}
	*t = 0;

	return (0);
}

/*------------------------------------------------------------------------
 * read in a line containing a term name
 */
static int tcap_load_term (FILE *fp, char *buf, const char *term)
{
	int		rc;

	/*--------------------------------------------------------------------
	 * read in lines looking for one which matches term name
	 */
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * read in next line
		 */
		rc = tcap_read_line(fp, buf);
		if (rc)
			break;

		/*----------------------------------------------------------------
		 * check if it contains term name
		 */
		rc = tcap_check_line(buf, term);
		if (rc == 0)
			return (0);
	}

	return (-1);
}

/*------------------------------------------------------------------------
 * read in a term entry, collecting any tc= references
 */
static int tcap_load_raw (char *buf, const char *term, const char *path)
{
	FILE *			fp = 0;
	char			tbuf[BUFSIZ];
	char			tc[128];
	char *			t;
	char *			p;
	const char **	ts;
	int				rc;

	/*--------------------------------------------------------------------
	 * get path of termcap file
	 */
	*tbuf = 0;
	if (path == 0 || *path == 0)
	{
		path = getenv("TERMCAP");
		if (path != 0)
		{
			/*------------------------------------------------------------
			 * bail if TERMCAP env var set to "none"
			 */
			if (tcap_trmcmp(path, "none") == 0)
				return (-1);

			/*------------------------------------------------------------
			 * if TERMCAP env var if not an absolute path, it may be
			 * the actual entry
			 */
			if (*path != '/')
			{
				p = strchr(path, ':');
				if (p != 0)
				{
					int l = (p - path);

					strncpy(tbuf, path, l);
					tbuf[l] = '|';
					strcpy(tbuf+l+1, path);
				}
				path = 0;
			}
		}
	}

	if (path != 0 && *path != 0)
		rc = tcap_path_exists(path);
	else
		rc = FALSE;

	if (! rc)
	{
		for (ts=tcap_tc_dirs; *ts; ts++)
		{
			path = *ts;
			rc = tcap_path_exists(path);
			if (rc)
				break;
		}
	}

	if (! rc)
		return (-1);

	/*--------------------------------------------------------------------
	 * open termcap file & read in entry if entry is empty
	 */
	if (*tbuf == 0)
	{
		fp = fopen(path, "r");
		if (fp == 0)
			return (-1);

		rc = tcap_load_term(fp, tbuf, term);
		if (rc)
		{
			fclose(fp);
			return (-1);
		}
	}

	/*--------------------------------------------------------------------
	 * copy entry to buffer, adding a ":DS=<description>" to the start
	 */
	t = strchr(tbuf, ':');
	if (t == 0)
	{
		fclose(fp);
		return (-1);
	}
	*t++ = 0;

	p = strrchr(tbuf, '|');
	if (p == 0)
	{
		if (fp != 0)
			fclose(fp);
		return (-1);
	}

	strcpy(buf, ":DS=");
	strcat(buf, p+1);
	strcat(buf, ":");
	strcat(buf, t);

	/*--------------------------------------------------------------------
	 * check if entry has a tc= entry
	 */
	rc = tcap_check_tc(buf, tc);
	if (rc)
	{
		if (fp != 0)
			fclose(fp);
		return (0);
	}

	/*--------------------------------------------------------------------
	 * if tc= entry found, chase them all down
	 */
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * either rewind our file pointer or open it if not yet open
		 */
		if (fp != 0)
		{
			rewind(fp);
		}
		else
		{
			fp = fopen(path, "r");
			if (fp == 0)
				return (-1);
		}

		/*----------------------------------------------------------------
		 * read in new entry
		 */
		rc = tcap_load_term(fp, tbuf, tc);
		if (rc)
			break;

		/*----------------------------------------------------------------
		 * append data from entry to buffer
		 */
		t = strchr(tbuf, ':');
		if (t == 0)
		{
			rc = -1;
			break;
		}

		strcat(buf, ++t);

		/*----------------------------------------------------------------
		 * now check if new entry has a tc= entry
		 */
		rc = tcap_check_tc(t, tc);
		if (rc)
		{
			rc = 0;
			break;
		}
	}

	fclose(fp);

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_tcgetent() - read in a term entry
 */
int tcap_tcgetent (char *buf, const char *term, const char *path)
{
	char	tbuf[TCAP_BUFLEN];
	char *	b;
	char *	t;
	int		rc;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (buf == 0 || term == 0 || *term == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * load in termcap entry
	 */
	rc = tcap_load_raw(tbuf, term, path);

	/*--------------------------------------------------------------------
	 * if entry found, compress extra white-space out of it
	 */
	if (rc == 0)
	{
		int		space = TRUE;

		t = tbuf;
		for (b=tbuf; *b; b++)
		{
			if (*b == ':')
			{
				space = TRUE;
				*t++ = *b;
			}
			else
			{
				if (isspace(*b))
				{
					if (! space)
						*t++ = *b;
				}
				else
				{
					space = FALSE;
					*t++ = *b;
				}
			}
		}

		if (t[-1] != ':')
			*t++ = ':';

		*t = 0;
	}

	/*--------------------------------------------------------------------
	 * now go through the buffer again, converting all colons to
	 * NULL chars and converting strings to true chars.
	 */
	if (rc == 0)
	{
		int	equal = FALSE;
		int	c;

		b = buf;
		t = tbuf;
		for (; *t; t++)
		{
			if (*t == ':')
			{
				*b++ = 0;
				equal = FALSE;
				continue;
			}

			if (equal)
			{
				for (; *t; t++)
				{
					if (*t == ':')
					{
						equal = FALSE;
						break;
					}

					switch (*t)
					{
					case '^':
						if (t[1] == ':')
						{
							*b++ = '^';
						}
						else if (t[1] == '^')
						{
							*b++ = *++t;
						}
						else if (t[1] == '?')
						{
							*b++ = 0x7f;
							t++;
						}
						else
						{
							c = *++t & 0x1f;
							if (c == 0)
								c = 0x80;
							*b++ = c;
						}
						break;

					case '\\':
						if (t[1] == ':')
						{
							*b++ = '\\';
						}
						else
						{
							t++;
							switch (*t)
							{
							case 'e':
							case 'E':
								*b++ = 0x1b;
								break;

							case '\\':
								*b++ = '\\';
								break;

							case '\n':
								*b++ = '\n';
								break;

							case 'r':
								*b++ = '\r';
								break;

							case 't':
								*b++ = '\t';
								break;

							case 'b':
								*b++ = '\b';
								break;

							case 'f':
								*b++ = '\f';
								break;

							case 's':
								*b++ = ' ';
								break;

							case '^':
								*b++ = '^';
								break;

							case '0':
							case '1':
							case '2':
							case '3':
							case '4':
							case '5':
							case '6':
							case '7':
								c = 0;
								while (TRUE)
								{
									if (*t == ':' || *t < '0' || *t > '7')
										break;
									c = (c * 8) + (*t++ - '0');
								}
								c &= 0xff;
								if (c == 0)
									c = 0x80;
								*b++ = c;
								break;

							default:
								*b++ = *t;
								break;
							}
						}
						break;

					default:
						*b++ = *t;
						break;
					}
				}
				*b++ = 0;

			}
			else
			{
				*b++ = *t;
				if (*t == '=')
					equal = TRUE;
			}
		}
		*b = 0;
	}

	return (rc);
}

/*------------------------------------------------------------------------
 * tcap_tcgetflag() - get a boolean entry from a termcap buffer
 */
int tcap_tcgetflag (const char *buf, const char *name)
{
	const char *	p;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (buf == 0 || name == 0 || *name == 0)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * find entry in buffer
	 */
	p = tcap_find_name(buf, name);
	if (p == 0)
		return (FALSE);

	/*--------------------------------------------------------------------
	 * boolean entries have nothing after the name
	 */
	if (*p != 0)
		return (FALSE);

	return (TRUE);
}

/*------------------------------------------------------------------------
 * tcap_tcgetnum() - get a numeric entry from a termcap buffer
 */
int tcap_tcgetnum (const char *buf, const char *name)
{
	const char *	p;
	int				num;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (buf == 0 || name == 0 || *name == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * find entry in buffer
	 */
	p = tcap_find_name(buf, name);
	if (p == 0)
		return (-1);

	/*--------------------------------------------------------------------
	 * numeric entries are in form "<name>#<value>"
	 */
	if (*p != '#')
		return (-1);

	/*--------------------------------------------------------------------
	 * collect numeric value
	 */
	num = 0;
	for (p++; *p; p++)
	{
		if (! isdigit(*p))
			break;

		num = (num * 10) + (*p - '0');
	}

	return (num);
}

/*------------------------------------------------------------------------
 * tcap_tcgetstr() - get string entry from a termcap buffer
 */
const char * tcap_tcgetstr (const char *buf, const char *name)
{
	const char *	p;

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	if (buf == 0 || name == 0 || *name == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * find entry in buffer
	 */
	p = tcap_find_name(buf, name);
	if (p == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * string entries are in form "<name>=<data>"
	 */
	if (*p != '=')
		return (0);

	/*--------------------------------------------------------------------
	 * return string
	 */
	return (p+1);
}

/*------------------------------------------------------------------------
 * tcap_tcgoto() - parameterize an entry
 */
char * tcap_tcgoto (char *buf, const char *str, const int *p)
{
	char *			b;
	const char *	s;
	int				n;
	int				add = 0;
	int				pp[2];

	/*--------------------------------------------------------------------
	 * sanity checks
	 */
	*buf = 0;
	if (str == 0 || *str == 0)
		return (buf);

	/*--------------------------------------------------------------------
	 * cache parameters in array
	 */
	pp[0] = p[0];
	pp[1] = p[1];
	n = 0;

	/*--------------------------------------------------------------------
	 * now process string
	 */
	b = buf;
	for (s=str; *s; s++)
	{
		if (*s != '%')
		{
			*b++ = *s;
		}
		else if (s[1] == 0)
		{
			*b++ = '%';
		}
		else
		{
			s++;
			switch (*s)
			{
			case '%':
				*b++ = '%';
				break;

			case 'r':
				pp[0] = p[1];
				pp[1] = p[0];
				break;

			case 'i':
				add = 1;
				break;

			case '.':
				if (n < 2)
				{
					*b++ = pp[n++];
				}
				break;

			case 'd':
				if (n < 2)
				{
					b += sprintf(b, "%d", pp[n++] + add);
				}
				break;

			case '2':
				if (n < 2)
				{
					b += sprintf(b, "%02d", pp[n++] + add);
				}
				break;

			case '3':
				if (n < 2)
				{
					b += sprintf(b, "%03d", pp[n++] + add);
				}
				break;

			case '+':
				s++;
				if (n < 2)
				{
					*b++ = *s + pp[n++];
				}
				break;

			default:
				break;
			}
		}
	}
	*b = 0;

	return (buf);
}

/*------------------------------------------------------------------------
 * tcap_tcputs() - output a command string
 */
int	tcap_tcputs (TERMINAL *tp, const char *str, int (*rtn)(TERMINAL *tp, int c))
{
	const unsigned char *	s;
	int						pad = 0;

	/*--------------------------------------------------------------------
	 * sanity check
	 */
	if (tp == 0)
		return (-1);

	if (str == 0 || *str == 0)
		return (0);

	/*--------------------------------------------------------------------
	 * collect any padding reqired
	 */
	s = (const unsigned char *)str;
	if (isdigit(*s))
	{
		for (; *s; s++)
		{
			if (! isdigit(*s))
				break;

			pad = (pad * 10) + (*s - '0');
		}

		if (*s == '*')
			s++;
	}

	/*--------------------------------------------------------------------
	 * now output the string
	 */
	for (; *s; s++)
	{
		(*rtn)(tp, *s);
	}

	/*--------------------------------------------------------------------
	 * now output any required padding
	 */
	if (pad > 0)
	{
		tcap_delay_output(tp, pad, rtn);
	}

	return (0);
}

/*------------------------------------------------------------------------
 * tcap_tcdump() - dump a termcap buffer to a stream
 */
void tcap_tcdump (const char *buf, const char *term, FILE *fp)
{
	const char *	p;
	const char *	b;
	const char *	s;

	/*--------------------------------------------------------------------
	 * tell what we are dumping
	 */
	fprintf(fp, "---------------------------------\n");
	fprintf(fp, "dump of termcap entry for \"%s\"\n", term);
	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the bools
	 */
	fprintf(fp, "bools:\n");

	b = buf;
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * get end of entry
		 */
		if (b[1] == 0)
			break;

		p = strchr(b + 1, 0);

		/*----------------------------------------------------------------
		 * check if a bool entry (no # or =)
		 */
		for (s = b+1; s < p; s++)
		{
			if (*s == '#' || *s == '=')
				break;
		}

		if (*s == 0)
		{
			fputc(' ', fp);
			fputc(' ', fp);
			for (s = b+1; s < p; s++)
			{
				fputc(*s, fp);
			}
			fputc('\n', fp);
		}

		b = p;
	}

	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the nums
	 */
	fprintf(fp, "nums:\n");

	b = buf;
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * get end of entry
		 */
		if (b[1] == 0)
			break;

		p = strchr(b + 1, 0);

		/*----------------------------------------------------------------
		 * check if a numeric entry (has a #)
		 */
		for (s = b+1; s < p; s++)
		{
			if (*s == '#' || *s == '=')
				break;
		}

		if (*s == '#')
		{
			fputc(' ', fp);
			fputc(' ', fp);
			for (s = b+1; s < p; s++)
			{
				if (*s == '#')
					fputs(" = ", fp);
				else
					fputc(*s, fp);
			}
			fputc('\n', fp);
		}

		b = p;
	}

	fprintf(fp, "\n");

	/*--------------------------------------------------------------------
	 * dump the strings
	 */
	fprintf(fp, "strs:\n");

	b = buf;
	while (TRUE)
	{
		/*----------------------------------------------------------------
		 * get end of entry
		 */
		if (b[1] == 0)
			break;

		p = strchr(b + 1, 0);

		/*----------------------------------------------------------------
		 * check if a string entry (has a =)
		 */
		for (s = b+1; s < p; s++)
		{
			if (*s == '#' || *s == '=')
				break;
		}

		if (*s == '=')
		{
			fputc(' ', fp);
			fputc(' ', fp);
			for (s = b+1; s < p; s++)
			{
				if (*s == '=')
				{
					fputs(" = ", fp);
				}
				else
				{
					int c = *s;

					if (c < 0x20)
					{
						switch (c)
						{
						case 0x1b:
							fputs("\\E", fp);
							break;

						case '\n':
							fputs("\\n", fp);
							break;

						case '\r':
							fputs("\\r", fp);
							break;

						case '\t':
							fputs("\\t", fp);
							break;

						case '\b':
							fputs("\\b", fp);
							break;

						case '\f':
							fputs("\\f", fp);
							break;

						case ' ':
							fputs("\\s", fp);
							break;

						case 0x1e:
							fputs("^^", fp);
							break;

						default:
							fprintf(fp, "\\%03o", c);
						}
					}
					else if (c == 0x7f)
					{
						fputs("^?", fp);
					}
					else
					{
						if (c == '\\')
							fputc(c, fp);
						fputc(c, fp);
					}
				}
			}
			fputc('\n', fp);
		}

		b = p;
	}

	fprintf(fp, "\n");
}
