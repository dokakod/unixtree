/*------------------------------------------------------------------------
 * Key name definition table
 *
 * This header is private to this library only.
 */
#ifndef TCAPKEYS_H
#define TCAPKEYS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * structure containing names of keys (NULL-entry terminated)
 *
 * Note that a given key-code may appear more than once (for key aliases),
 * but the first one found is used for code-to-name lookup.
 */
struct	key_def
{
	const char *	key_name;		/* key name		*/
	int				key_code;		/* key code		*/
};
typedef struct key_def KEY_DEF;

extern const KEY_DEF	tcap_key_tbl[];

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPKEYS_H */
