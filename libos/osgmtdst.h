/*------------------------------------------------------------------------
 * time functions
 */
#ifndef OSGMTDST_H
#define OSGMTDST_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * functions
 */
extern time_t os_get_gmt	(void);
extern time_t os_get_dst	(time_t t);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* OSGMTDST_H */
