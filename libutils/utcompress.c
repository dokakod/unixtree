/*------------------------------------------------------------------------
 * compress.c - File compression ala IEEE Computer, June 1984.
 */
#include "utcommon.h"

/*------------------------------------------------------------------------
 * misc defines
 */
#ifndef min_bits
#define min_bits(a,b)	((a) < (b) ? (a) : (b))
#endif

#define BITS			16

#define INIT_BITS		9			/* initial number of bits/code */

#define HSIZE_16		69001		/* 95% occupancy */
#define HSIZE_155		50021
#define HSIZE_15		35023		/* 94% occupancy */
#define HSIZE_14		18013		/* 91% occupancy */
#define HSIZE_13		9001		/* 91% occupancy */
#define HSIZE_12		5003		/* 80% occupancy */

#define MAXCODE(n_bits)	((1L << (n_bits)) - 1)

/* Defines for third byte of header (bit mask & flags) */

#define BIT_MASK		0x1f
#define BLOCK_MASK		0x80

/* Mask bits 0x40 and 0x20 are free.
 * I think 0x20 should mean that there is a fourth header byte
 * (for expansion).
 */

/*
 * The next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */

#define FIRST		257				/* first free entry */
#define	CLEAR		256				/* table clear output code */

#define CHECK_GAP	10000			/* ratio check interval */

/*------------------------------------------------------------------------
 * misc typedefs
 */
typedef	unsigned char	char_type;

/*
 * a code_int must be able to hold 2**BITS values of type int, and also -1
 */
typedef int		code_int;
typedef int		count_int;

/*------------------------------------------------------------------------
 * static data
 */
typedef struct comp_data
{
	int					n_bits;			/* number of bits/code */
	int					maxbits;		/* user settable max # bits/code */
	code_int			maxcode;		/* maximum code, given n_bits */
	code_int			maxmaxcode;		/* should NEVER generate this */

	code_int			hsize;			/* size of htab & codetab */
	count_int *			htab;
	unsigned short *	codetab;

	int					offset;
	int					size;
	char_type			inpbuf[BITS];
	char				outbuf[BITS];

	code_int			free_ent;		/* first unused entry */

	int					clear_flg;
	long				ratio;
	count_int			checkpoint;
	int					block_compress;

	long				bytes_in;		/* length of input */
	long				bytes_out;		/* length of compressed output */
	long				out_count;		/* # of codes output (for debugging) */
} COMP_DATA;

/*------------------------------------------------------------------------
 * To save much memory, we overlay the table used by compress() with those
 * used by decompress().  The tab_prefix table is the same size and type
 * as the codetab.  The tab_suffix table needs 2**BITS characters.  We
 * get this from the beginning of htab.  The output stack uses the rest
 * of htab, and contains characters.  There is plenty of room for any
 * possible stack (stack used to be 8000 characters).
 */
#define htabof(i)		cd->htab[i]
#define codetabof(i)	cd->codetab[i]

#define tab_prefixof(i)	codetabof(i)
#define tab_suffixof(i)	((char_type *)(cd->htab))[i]
#define de_stack		((char_type *)&tab_suffixof(1<<BITS))

/*------------------------------------------------------------------------
 * const tables
 */
static const char_type lmask[9] =
{
	0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00
};

static const char_type rmask[9] =
{
	0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff
};

static const char_type magic_header[] = { 0x1f, 0x9d };

/*------------------------------------------------------------------------
 * reset code table
 */
static int cl_hash (COMP_DATA *cd, register count_int hsize)
{
	register count_int *htab_p = cd->htab+hsize;
	register long i;
	register long m1 = -1;

	i = hsize - 16;
 	do
	{				/* might use Sys V memset(3) here */
		*(htab_p-16) = m1;
		*(htab_p-15) = m1;
		*(htab_p-14) = m1;
		*(htab_p-13) = m1;
		*(htab_p-12) = m1;
		*(htab_p-11) = m1;
		*(htab_p-10) = m1;
		*(htab_p- 9) = m1;
		*(htab_p- 8) = m1;
		*(htab_p- 7) = m1;
		*(htab_p- 6) = m1;
		*(htab_p- 5) = m1;
		*(htab_p- 4) = m1;
		*(htab_p- 3) = m1;
		*(htab_p- 2) = m1;
		*(htab_p- 1) = m1;
		htab_p -= 16;
	} while ((i -= 16) >= 0);
	for ( i += 16; i > 0; i-- )
		*--htab_p = m1;

	return (0);
}

/*------------------------------------------------------------------------
 * table clear for block compress
 */
static int cl_block (COMP_DATA *cd)
{
	register long rat;
	int rc = 0;

	cd->checkpoint = cd->bytes_in + CHECK_GAP;

	if (cd->bytes_in > 0x007fffff)
	{	/* shift will overflow */
		rat = cd->bytes_out >> 8;
		if (rat == 0)
		{		/* Don't divide by zero */
			rat = 0x7fffffff;
		}
		else
		{
			rat = cd->bytes_in / rat;
		}
	}
	else
	{
		rat = (cd->bytes_in << 8) / cd->bytes_out;	/* 8 fractional bits */
	}

	if ( rat > cd->ratio )
	{
		cd->ratio = rat;
	}
	else
	{
		cd->ratio = 0;
 		cl_hash (cd, (count_int) cd->hsize );
		cd->free_ent = FIRST;
		cd->clear_flg = 1;
		rc = 1;
	}
	return (rc);
}

/*------------------------------------------------------------------------
 * TAG( output )
 *
 * Output the given code.
 * Inputs:
 * 	code:	A n_bits-bit integer.  If == -1, then EOF.  This assumes
 *		that n_bits =< (long)wordsize - 1.
 * Outputs:
 * 	Outputs code to the file.
 * Assumptions:
 *	Chars are 8 bits long.
 * Algorithm:
 * 	Maintain a BITS character long buffer (so that 8 codes will
 * fit in it exactly).  Use the VAX insv instruction to insert each
 * code in turn.  When the buffer fills up empty it and start over.
 */
static int output (COMP_DATA *cd, FILE *fp, code_int code)
{
	register int r_off = cd->offset;
	register int bits  = cd->n_bits;
	register char * bp = cd->outbuf;

	if ( code >= 0 )
	{
		/*
		 * byte/bit numbering on the VAX is simulated by the following code
		 */
		/*
		 * Get to the first byte.
		 */
		bp += (r_off >> 3);
		r_off &= 7;
		/*
		 * Since code is always >= 8 bits, only need to mask the first
		 * hunk on the left.
		 */
		*bp = (int)((*bp & rmask[r_off]) | (code << r_off) & lmask[r_off]);
		bp++;
		bits -= (8 - r_off);
		code >>= 8 - r_off;
		/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
		if ( bits >= 8 )
		{
			*bp++ = (int)code;
			code >>= 8;
			bits -= 8;
		}
		/* Last bits. */
		if (bits)
			*bp = (int)code;

		cd->offset += cd->n_bits;
		if ( cd->offset == (cd->n_bits << 3) )
		{
			bp = cd->outbuf;
			bits = cd->n_bits;
			cd->bytes_out += bits;
			do
				putc(*bp++, fp);
			while (--bits);
			cd->offset = 0;
		}

		/*
		 * If the next entry is going to be too big for the code size,
		 * then increase it, if possible.
		 */
		if ( cd->free_ent > cd->maxcode || (cd->clear_flg > 0))
		{
			/*
			 * Write the whole buffer, because the input side won't
			 * discover the size increase until after it has read it.
			 */
			if ( cd->offset > 0 )
			{
				fwrite( cd->outbuf, 1, cd->n_bits, fp );
				cd->bytes_out += cd->n_bits;
			}
			cd->offset = 0;

			if ( cd->clear_flg )
			{
				cd->maxcode = MAXCODE (cd->n_bits = INIT_BITS);
				cd->clear_flg = 0;
			}
			else
			{
				cd->n_bits++;
				if ( cd->n_bits == cd->maxbits )
					cd->maxcode = cd->maxmaxcode;
				else
					cd->maxcode = MAXCODE(cd->n_bits);
			}
		}
	}
	else
	{
		/*
		 * At EOF, write the rest of the buffer.
		 */
		if ( cd->offset > 0 )
			fwrite( cd->outbuf, 1, (cd->offset + 7) / 8, fp );
		cd->bytes_out += (cd->offset + 7) / 8;
		cd->offset = 0;
		fflush( fp );
	}

	return (0);
}

/*------------------------------------------------------------------------
 * TAG( get_code )
 *
 * Read one code from the fp.  If EOF, return -1.
 * Inputs:
 * 	fp
 * Outputs:
 * 	code or -1 is returned.
 */
static code_int get_code (COMP_DATA *cd, FILE *fp)
{
	/*
	 * On the VAX, it is important to have the register declarations
	 * in exactly the order given, or the asm will break.
	 */
	register code_int code;
	register int r_off, bits;
	register char_type *bp = cd->inpbuf;

	if ( cd->clear_flg > 0 ||
		cd->offset >= cd->size || cd->free_ent > cd->maxcode )
	{
		/*
		 * If the next entry will be too big for the current code
		 * size, then we must increase the size.  This implies reading
		 * a new buffer full, too.
		 */
		if ( cd->free_ent > cd->maxcode )
		{
			cd->n_bits++;
			if ( cd->n_bits == cd->maxbits )
				cd->maxcode = cd->maxmaxcode;	/* won't get any bigger now */
			else
				cd->maxcode = MAXCODE(cd->n_bits);
		}
		if ( cd->clear_flg > 0)
		{
			cd->maxcode = MAXCODE (cd->n_bits = INIT_BITS);
			cd->clear_flg = 0;
		}
		cd->size = fread( cd->inpbuf, 1, cd->n_bits, fp );
		if ( cd->size <= 0 )
			return (-1);			/* end of file */
		cd->offset = 0;
		/* Round size down to integral number of codes */
		cd->size = (cd->size << 3) - (cd->n_bits - 1);
	}
	r_off = cd->offset;
	bits = cd->n_bits;
	/*
	 * Get to the first byte.
	 */
	bp += (r_off >> 3);
	r_off &= 7;
	/* Get first part (low order bits) */
	code = (*bp++ >> r_off);
	bits -= (8 - r_off);
	r_off = 8 - r_off;		/* now, offset into code word */
	/* Get any 8 bit parts in the middle (<=1 for up to 16 bits). */
	if ( bits >= 8 )
	{
		code |= *bp++ << r_off;
		r_off += 8;
		bits -= 8;
	}
	/* high order bits. */
	code |= (*bp & rmask[bits]) << r_off;
	cd->offset += cd->n_bits;

	return (code);
}

/*------------------------------------------------------------------------
 * compress fp_inp to fp_out
 *
 * Algorithm:  use open addressing double hashing (no chaining) on the
 * prefix code / next character combination.  We do a variant of Knuth's
 * algorithm D (vol. 3, sec. 6.4) along with G. Knott's relatively-prime
 * secondary probe.  Here, the modular division first probe gives way
 * to a faster exclusive-or manipulation.  Also do block compression with
 * an adaptive reset, whereby the code table is cleared when the compression
 * ratio decreases, but after the table fills.  The variable-length output
 * codes are re-sized at this point, and a special CLEAR code is generated
 * for the decompressor.  Late addition:  construct the table according to
 * file size for noticeable speed improvement on small files.
 */
static int compress (FILE *fp_inp, FILE *fp_out, long fsize)
{
	COMP_DATA comp_data;
	COMP_DATA *cd = &comp_data;
	register long fcode;
	register code_int i = 0;
	register int c;
	register code_int ent;
	register int disp;
	register code_int hsize_reg;
	register int hshift;
	int rc;

	cd->block_compress = BLOCK_MASK;
	cd->maxbits = BITS;
	cd->maxmaxcode = 1L << cd->maxbits;
	if (fsize < 0)
		fsize = 100000;

	/* get data arrays */

	switch (cd->maxbits)
	{
	case 16:	cd->hsize = HSIZE_16;
				break;

	case 15:	cd->hsize = HSIZE_15;
				break;

	case 14:	cd->hsize = HSIZE_14;
				break;

	case 13:	cd->hsize = HSIZE_13;
				break;

	default:	cd->hsize = HSIZE_12;
				break;
	}

	if ( fsize < (1 << 12) )
		cd->hsize = min_bits(cd->hsize, HSIZE_12);
	else if ( fsize < (1 << 13) )
		cd->hsize = min_bits(cd->hsize, HSIZE_13);
	else if ( fsize < (1 << 14) )
		cd->hsize = min_bits(cd->hsize, HSIZE_14);
	else if ( fsize < (1 << 15) )
		cd->hsize = min_bits(cd->hsize, HSIZE_15);
	else if ( fsize < 47000 )
		cd->hsize = min_bits(cd->hsize, HSIZE_155);

	cd->htab    = (count_int      *)
					MALLOC((int)cd->hsize * (int)sizeof(*cd->htab));
	cd->codetab = (unsigned short *)
					MALLOC((int)cd->hsize * (int)sizeof(*cd->codetab));

	cd->bytes_out = 0;

	putc(magic_header[0], fp_out);
	putc(magic_header[1], fp_out);
	putc((char)(cd->maxbits | cd->block_compress), fp_out);
	cd->bytes_out = 3;

	cd->offset = 0;
	cd->size = 0;
	cd->out_count = 0;
	cd->clear_flg = 0;
	cd->ratio = 0;
	cd->bytes_in = 1;
	cd->checkpoint = CHECK_GAP;
	cd->maxcode = MAXCODE(cd->n_bits = INIT_BITS);
	cd->free_ent = ((cd->block_compress) ? FIRST : 256 );

	ent = getc(fp_inp);

	hshift = 0;
	for ( fcode = (long) cd->hsize;  fcode < 65536L; fcode *= 2L )
		hshift++;
	hshift = 8 - hshift;		/* set hash code range bound */

	hsize_reg = cd->hsize;
	cl_hash(cd, (count_int) hsize_reg);		/* clear hash table */

	while ( (c = getc(fp_inp)) != EOF )
	{
		cd->bytes_in++;
		fcode = (long) (((long) c << cd->maxbits) + ent);
 		i = (((long)c << hshift) ^ ent);	/* xor hashing */

		if ( htabof (i) == fcode )
		{
			ent = codetabof (i);
			continue;
		}
		else if ( (long)htabof (i) < 0 )	/* empty slot */
			goto nomatch;
 		disp = (int)(hsize_reg - i);		/* secondary hash */
		if ( i == 0 )
			disp = 1;
probe:
		if ( (i -= disp) < 0 )
			i += hsize_reg;

		if ( htabof (i) == fcode )
		{
			ent = codetabof (i);
			continue;
		}
		if ( (long)htabof (i) > 0 )
			goto probe;
nomatch:
		output(cd, fp_out, (code_int) ent );
		cd->out_count++;
 		ent = c;
		if ( cd->free_ent < cd->maxmaxcode )
		{
 	    	codetabof (i) = (unsigned short)cd->free_ent++;
			htabof (i) = fcode;
		}
		else if ( (count_int)cd->bytes_in >= cd->checkpoint &&
			cd->block_compress )
		{
			if (cl_block(cd))
				output(cd, fp_out, (code_int)CLEAR );
		}
	}
   	/*
	 * Put out the final code.
	 */
   	output(cd, fp_out, (code_int)ent );
   	cd->out_count++;
   	output(cd, fp_out, (code_int)-1 );

	rc = 0;
   	if (cd->bytes_out > cd->bytes_in)	/* check if no savings */
		rc = 1;

	FREE(cd->htab);
	FREE(cd->codetab);
   	return (rc);
}

/*------------------------------------------------------------------------
 * Decompress fp_inp to fp_out.  This routine adapts to the codes in the
 * file building the "string" table on-the-fly; requiring no table to
 * be stored in the compressed file.  The tables used herein are shared
 * with those of the compress() routine.  See the definitions above.
 */
static int decompress (FILE *fp_inp, FILE *fp_out, long fsize)
{
	COMP_DATA comp_data;
	COMP_DATA *cd = &comp_data;
	register char_type *stackp;
	register int finchar;
	register code_int code, oldcode, incode;

	/* Check the magic number */

	if ((getc(fp_inp) != (magic_header[0] & 0xff)) ||
		(getc(fp_inp) != (magic_header[1] & 0xff)) )
	{
		return (2);
	}
	cd->maxbits = getc(fp_inp);	/* set -b from file */
	cd->block_compress = cd->maxbits & BLOCK_MASK;
	cd->maxbits &= BIT_MASK;
	if (cd->maxbits > BITS)
	{
		return (2);
	}
	cd->maxmaxcode = 1L << cd->maxbits;

	cd->size = 0;
	if (fsize < 0)
		fsize = 100000;

	/* get data arrays */

	switch (cd->maxbits)
	{
	case 16:	cd->hsize = HSIZE_16;
				break;

	case 15:	cd->hsize = HSIZE_15;
				break;

	case 14:	cd->hsize = HSIZE_14;
				break;

	case 13:	cd->hsize = HSIZE_13;
				break;

	default:	cd->hsize = HSIZE_12;
				break;
	}

	cd->htab    = (count_int      *)
					MALLOC(((int)cd->hsize * (int)sizeof(*cd->htab)));
	cd->codetab = (unsigned short *)
					MALLOC(((int)cd->hsize * (int)sizeof(*cd->codetab)));

	/*
	 * As above, initialize the first 256 entries in the table.
	 */
	cd->maxcode = MAXCODE(cd->n_bits = INIT_BITS);
	for ( code = 255; code >= 0; code-- )
	{
		tab_prefixof(code) = 0;
		tab_suffixof(code) = (char_type)code;
	}
	cd->free_ent = ((cd->block_compress) ? FIRST : 256 );

	oldcode = get_code(cd, fp_inp);
	finchar = (int)oldcode;
	if (oldcode == -1)	/* EOF already? */
		goto decompress_done;
	putc( (char)finchar, fp_out );		/* first code must be 8 bits = char */
	stackp = de_stack;

	while ( (code = get_code(cd, fp_inp)) > -1 )
	{
		if ( (code == CLEAR) && cd->block_compress )
		{
			for ( code = 255; code >= 0; code-- )
				tab_prefixof(code) = 0;
			cd->clear_flg = 1;
			cd->free_ent = FIRST - 1;
			if ( (code = get_code (cd, fp_inp)) == -1 )	/* untimely death */
				break;
		}
		incode = code;
		/*
		 * Special case for KwKwK string.
		 */
		if ( code >= cd->free_ent )
		{
			*stackp++ = (char_type)finchar;
			code = oldcode;
		}

		/*
		 * Generate output characters in reverse order
		 */
		while ( code >= 256 )
		{
			*stackp++ = tab_suffixof(code);
			code = tab_prefixof(code);
		}
		*stackp++ = finchar = tab_suffixof(code);

		/*
		 * And put them out in forward order
		 */
		do
			putc( *--stackp, fp_out );
		while ( stackp > de_stack );

		/*
		 * Generate the new entry.
		 */
		if ( (code=cd->free_ent) < cd->maxmaxcode )
		{
			tab_prefixof(code) = (unsigned short)oldcode;
			tab_suffixof(code) = (char_type)finchar;
			cd->free_ent = code+1;
		}
		/*
		 * Remember previous code.
		 */
		oldcode = incode;
	}
	fflush( fp_out );

decompress_done:
	FREE(cd->htab);
	FREE(cd->codetab);

	return (0);
}

/*------------------------------------------------------------------------
 * comp() - compress a path to a path
 */
int comp (const char *path_inp, const char *path_out)
{
	FILE *fp_inp;
	FILE *fp_out;
	long fsize;
	struct stat stbuf;

	fp_inp = fopen(path_inp, "rb");
	if (fp_inp == 0)
	{
		return (-1);
	}

	fp_out = fopen(path_out, "wb");
	if (fp_out == 0)
	{
		fclose(fp_inp);
		return (-1);
	}

	os_fstat(fileno(fp_inp), &stbuf);
	fsize = stbuf.st_size;

	if (fsize)
		compress(fp_inp, fp_out, fsize);

	fclose(fp_inp);
	fclose(fp_out);

	os_chmod(path_out, stbuf.st_mode & 0777);

	return (0);
}

/*------------------------------------------------------------------------
 * decomp() - decompress a path to a path
 */
int decomp (const char *path_inp, const char *path_out)
{
	FILE *	fp_inp;
	FILE *	fp_out;
	long	fsize;
	struct	stat stbuf;

	fp_inp = fopen(path_inp, "rb");
	if (fp_inp == 0)
	{
		return (-1);
	}

	fp_out = fopen(path_out, "wb");
	if (fp_out == 0)
	{
		fclose(fp_inp);
		return (-1);
	}

	os_fstat(fileno(fp_inp), &stbuf);
	fsize = stbuf.st_size;

	if (fsize)
		decompress(fp_inp, fp_out, fsize);

	fclose(fp_inp);
	fclose(fp_out);

	os_chmod(path_out, stbuf.st_mode & 0777);

	return (0);
}
