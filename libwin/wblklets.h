/*------------------------------------------------------------------------
 * defines for block letters
 */
#ifndef WBLKLETS_H
#define WBLKLETS_H

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
extern "C" {
#endif

/*------------------------------------------------------------------------
 * block letter struct
 */
typedef char BLK_LINE[BLK_WIDTH + 1];

struct blk_lets
{
	BLK_LINE	line[BLK_HEIGHT];
};
typedef struct blk_lets BLK_LETS;

/*------------------------------------------------------------------------
 * block letter tables
 */
extern const BLK_LETS blk_ltrs[];
extern const BLK_LETS blk_nums[];

/*------------------------------------------------------------------------
 * C++ stuff
 */
#ifdef __cplusplus
}
#endif

#endif /* WBLKLETS_H */
