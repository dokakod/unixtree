/*------------------------------------------------------------------------
 *	header for clock routines
 */
#ifndef TERMCLOCK_H
#define TERMCLOCK_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * clock routine callback type
 */
typedef void CLOCK_RTN (time_t clock, void *data);

/*------------------------------------------------------------------------
 * function prototypes
 */
extern void		term_clock_init		(int timeval, CLOCK_RTN *rtn, void *data);
extern void		term_clock_clear	(void);

extern void		term_clock_set		(int bf);
extern void		term_clock_on		(void);
extern void		term_clock_off		(void);
extern int		term_clock_active	(int bf);

extern int		term_clock_check	(void);
extern void		term_clock_trip		(void);

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* TERMCLOCK_H */
