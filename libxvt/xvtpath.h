/*------------------------------------------------------------------------
 * pathname utility functions
 */
#ifndef XVTPATH_H
#define XVTPATH_H

extern const char *	xvt_path_basename	(const char *path);
extern char *		xvt_path_dirname	(char *dirname,  const char *path);
extern const char *	xvt_path_extension	(const char *path);

extern int			xvt_path_find		(char *fullpath, const char *filename);
extern char *		xvt_path_resolve	(char *fullpath, const char *skeleton);

#endif /* XVTPATH_H */
