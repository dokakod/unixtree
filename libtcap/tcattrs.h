/*------------------------------------------------------------------------
 * header for attrs routines
 *
 * This header is public.
 */
#ifndef TCATTRS_H
#define TCATTRS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * function prototypes
 */
extern int			tcap_get_key_value			(const char *name);

extern const char *	tcap_get_key_name			(int    code, char *buf);
extern const char *	tcap_get_char_name			(int    code, char *buf);

extern const char *	tcap_get_attr_name_by_code	(attr_t code, char *buf);
extern const char *	tcap_get_color_name_by_code	(attr_t code, char *buf);
extern const char * tcap_get_fg_name_by_code	(attr_t code);
extern const char * tcap_get_bg_name_by_code	(attr_t code);

extern const char *	tcap_get_attr_name_by_num	(int n);
extern const char *	tcap_get_fg_name_by_num		(int n);
extern const char *	tcap_get_bg_name_by_num		(int n);

extern attr_t		tcap_get_attr_value_by_num	(int n);
extern attr_t		tcap_get_fg_value_by_num	(int n);
extern attr_t		tcap_get_bg_value_by_num	(int n);

extern int			tcap_get_attr_num_by_name	(const char *name);
extern int			tcap_get_fg_num_by_name		(const char *name);
extern int			tcap_get_bg_num_by_name		(const char *name);

extern int			tcap_get_attr_num_by_code	(attr_t code);
extern int			tcap_get_fg_num_by_code		(attr_t code);
extern int			tcap_get_bg_num_by_code		(attr_t code);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCATTRS_H */
