/*------------------------------------------------------------------------
 * print info
 */
#ifndef PRINT_H
#define PRINT_H

/*------------------------------------------------------------------------
 * print info
 *
 * This data is static because the PRINTER may be kept open during a session.
 */
struct print_info
{
	PRINTER *	prt;
	int			page_no;
	int			linecnt;
	char		date_string[24];
	char		print_name[MAX_PATHLEN];
};
typedef struct print_info PRINT_INFO;

#endif /* PRINT_H */
