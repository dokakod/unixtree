/*------------------------------------------------------------------------
 * dump the headers in a resource file
 */
#include "restool.h"

/*------------------------------------------------------------------------
 * dump the headers in a resource file
 */
int res_headers (const char *name, char *msgbuf)
{
	char		res_path[256];
	RES_FILE *	rf;
	int			i;

	/*--------------------------------------------------------------------
	 * load the resource file
	 */
	sprintf(res_path, "%s.res", name);

	rf = res_file_load(res_path, 1);
	if (rf == 0)
	{
		sprintf(msgbuf, "Cannot load resource file %s", res_path);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * dump the contents
	 */
	printf("%-8s: %s\n", "name", rf->hdr.name);

	for (i=0; i<rf->hdr.num_ents; i++)
	{
		const RES_TYPE *	rt;

		rt = res_find_by_type(rf->entries[i].type);

		printf("%-8s: type %s: count %4d\n",
			rf->entries[i].name,
			rt->type_name,
			rf->entries[i].num);
	}

	/*--------------------------------------------------------------------
	 * free the struct
	 */
	res_file_free(rf);

	return (0);
}
