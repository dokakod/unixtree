/*------------------------------------------------------------------------
 * user/group name/id routines
 */
#ifndef OSUSRGRP_H
#define OSUSRGRP_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int		os_get_usr_id_from_name		(const char *name);
extern int		os_get_grp_id_from_name		(const char *name);
extern char *	os_get_usr_name_from_id		(int id, char *buf);
extern char *	os_get_grp_name_from_id		(int id, char *buf);
extern char *	os_get_usr_home				(const char *name, char *buf);
extern char *	os_get_usr_name				(char *buf);

extern int		os_get_uid					(void);
extern int		os_get_gid					(void);
extern int		os_get_euid					(void);
extern int		os_get_egid					(void);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSUSRGRP_H */
