/*------------------------------------------------------------------------
 * libtcap definitions
 */
#ifndef TCAPDEFS_H
#define TCAPDEFS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * asyncronous-key routine
 */
typedef int ASYNC_RTN (int key, void *data);

/*------------------------------------------------------------------------
 * cursor visibility values
 *
 * These values are assumed by "standard" curses, but it provides no
 * defines of names for the values.
 */
#define VISIBILITY_OFF		0			/* cursor invisible		*/
#define VISIBILITY_ON		1			/* cursor visible		*/
#define VISIBILITY_VERY_ON	2			/* cursor very visible	*/

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TCAPDEFS_H */
