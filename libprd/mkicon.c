/*------------------------------------------------------------------------
 * this program creates an icon file from a text file
 *
 *	The text file is in form:
 *
 *		line 1:		name
 *		line 2:		width	(in bits)
 *		line 3:		height	(in bits)
 *		line n:		data	(...x...)
 *
 *	Comment lines (starting with a #) are ignored.
 *
 *	Usage: mkicon [file]
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

struct icon
{
	char			name[128];
	int				width;
	int				height;
	unsigned char	bits[64 * 64];
};
typedef struct icon ICON;

static void strip (char *line)
{
	char *	end = line + (strlen(line) - 1);

	for (; end >= line; end--)
	{
		if (! isspace(*end))
			break;
	}
	end[1] = 0;
}

static int read_line (char *line, int len, FILE *fp)
{
	for (;;)
	{
		if (fgets(line, len, fp) == 0)
			return (-1);

		strip(line);
		if (*line == 0 || *line == '#')
			continue;
		break;
	}

	return (0);
}

static int icon_read (ICON *icon, FILE *fp)
{
	char			line[128];
	unsigned char *	b;
	int				l;

	/*--------------------------------------------------------------------
	 * read in name
	 */
	if (read_line(line, sizeof(line), fp) != 0)
	{
		fprintf(stderr, "Unexpected EOF reading name\n");
		return (-1);
	}

	strcpy(icon->name, line);

	/*--------------------------------------------------------------------
	 * read in width
	 */
	if (read_line(line, sizeof(line), fp) != 0)
	{
		fprintf(stderr, "Unexpected EOF reading width\n");
		return (-1);
	}

	icon->width = atoi(line);
	if (icon->width <= 0 || icon->width > 64 || (icon->width % 8) != 0)
	{
		fprintf(stderr, "Invalid width %d\n", icon->width);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * read in height
	 */
	if (read_line(line, sizeof(line), fp) != 0)
	{
		fprintf(stderr, "Unexpected EOF reading height\n");
		return (-1);
	}

	icon->height = atoi(line);
	if (icon->height <= 0 || icon->height > 64 || (icon->height % 8) != 0)
	{
		fprintf(stderr, "Invalid height %d\n", icon->height);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * read in lines
	 */
	b = icon->bits;
	for (l=0; l<icon->height; l++)
	{
		int		m;
		int		j;
		int		c;

		/*----------------------------------------------------------------
		 * read in next data line
		 */
		if (read_line(line, sizeof(line), fp) != 0)
		{
			fprintf(stderr, "Unexpected EOF reading line %d\n", l+1);
			return (-1);
		}

		/*----------------------------------------------------------------
		 * check if valid
		 */
		if ((int)strlen(line) != icon->width)
		{
			fprintf(stderr, "Data line %d invalid\n", l+1);
			return (-1);
		}

		/*----------------------------------------------------------------
		 * convert bits
		 */
		m = 1;
		c = 0;
		for (j=0; ; j++)
		{
			if (j > 0 && (j % 8) == 0)
			{
				*b++ = c;
				c = 0;
				m = 1;

				if (j == icon->width)
					break;
			}

			if (line[j] != ' ' && line[j] != '.')
				c |= m;
			m <<= 1;
		}
	}

	return (0);
}

static int icon_write (ICON *icon)
{
	char			filename[128];
	FILE *			fp;
	unsigned char *	b;
	int				l;
	int				bytes = icon->width / 8;

	/*--------------------------------------------------------------------
	 * open output file
	 */
	strcpy(filename, icon->name);
	strcat(filename, ".xbm");
	fp = fopen(filename, "w");
	if (fp == 0)
	{
		fprintf(stderr, "Cannot open file %s\n", filename);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * write file header
	 */
	fprintf(fp, "/* ALL EDITS WILL BE LOST - This is a generated file. */\n");
	fprintf(fp, "\n");
	fprintf(fp, "#define %s_icon_width\t%d\n",  icon->name, icon->width);
	fprintf(fp, "#define %s_icon_height\t%d\n", icon->name, icon->height);
	fprintf(fp, "\n");
	fprintf(fp, "static unsigned char %s_icon_bits[] =\n",
		icon->name);
	fprintf(fp, "{\n");

	/*--------------------------------------------------------------------
	 * write data lines
	 */
	b = icon->bits;
	for (l=0; l<icon->height; l++)
	{
		int w;

		fprintf(fp, "\t");
		for (w=0; w < bytes; w++)
		{
			fprintf(fp, "0x%02x", *b++);
			if (l != icon->height -1 || w != bytes - 1)
				fprintf(fp, ",");
		}
		fprintf(fp, "\n");
	}

	/*--------------------------------------------------------------------
	 * write file trailer
	 */
	fprintf(fp, "};\n");

	fclose(fp);

	return (0);
}

int main (int argc, char **argv)
{
	FILE *	fp	= stdin;
	ICON	icon;
	int		rc;

	/*-----------------------------------------------------------
	 * check for help
	 */
	if (argc > 1)
	{
		if (strcmp(argv[1], "-?")     == 0 ||
	    	strcmp(argv[1], "-help")  == 0 ||
	    	strcmp(argv[1], "--help") == 0)
		{
			printf("usage: %s [file]\n", argv[0]);
			return (0);
		}
	}

	/*-----------------------------------------------------------
	 * check if file specified
	 */
	if (argc > 1)
	{
		fp = fopen(argv[1], "r");
		if (fp == 0)
		{
			fprintf(stderr, "%s: Cannot open %s\n", argv[0], argv[1]);
			return (1);
		}
	}

	/*-----------------------------------------------------------
	 * read in icon text file
	 */
	rc = icon_read(&icon, fp);
	if (fp != stdin)
		fclose(fp);

	/*-----------------------------------------------------------
	 * write out icon data file
	 */
	if (rc == 0)
		rc = icon_write(&icon);

	return (rc == 0 ? 0 : 1);
}
