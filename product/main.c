/*------------------------------------------------------------------------
 * This is the actual main entry point
 *
 * We cheat here & declare our internal main rather than
 * load umpteen headers.
 */

extern int int_main (int argc, char **argv);

int main (int argc, char **argv)
{
	int rc;

	/*--------------------------------------------------------------------
	 * call the real main
	 */
	rc = int_main(argc, argv);

	/*--------------------------------------------------------------------
	 * we never actually get here
	 */
	return (rc == 0 ? 0 : 1);
}
