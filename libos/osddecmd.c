/*------------------------------------------------------------------------
 * routine to get the full cmd for a DDE link
 */
#include "oscommon.h"

#if V_WINDOWS

static void
os_dde_cmd_fill (const char *path, const char *buffer, char *cmd)
{
	const char *p;

	p = strstr(buffer, "%1");
	if (p == 0)
		p = strstr(buffer, "%L");

	if (p == 0)
	{
		strcpy(cmd, buffer);
		strcat(cmd, " \"");
		strcat(cmd, path);
		strcat(cmd, "\"");
	}
	else
	{
		strncpy(cmd, buffer, p-buffer);
		strcat(cmd, path);
		strcat(cmd, p+2);
	}
}

int
os_dde_cmd_get (const char *path, char *cmd)
{
	const char *	ext;
	HKEY			hClasses;
	HKEY			hExtension;
	HKEY			hCommand;
	LONG			lRC;
	char			Data[1024];
	DWORD			dwDatalen;
	DWORD			dwType;

	/*--------------------------------------------------------------------
	 * get extension of file
	 */
	ext = fn_ext(path);
	if (ext == 0 || *ext == 0)
		return (-1);

	ext--;		/* point to ".xxx */

	/*--------------------------------------------------------------------
	 * open classes section of registry
	 */
	lRC = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "Software\\Classes", 0,
		KEY_READ, &hClasses);
	if (lRC != ERROR_SUCCESS)
		return (-1);

	/*--------------------------------------------------------------------
	 * open extension section of registry
	 */
	lRC = RegOpenKeyEx(hClasses, ext, 0, KEY_READ, &hExtension);
	if (lRC != ERROR_SUCCESS)
	{
		RegCloseKey(hClasses);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * get default entry
	 */
	dwDatalen = sizeof(Data);
	lRC = RegQueryValueEx(hExtension, NULL, NULL, &dwType, Data, &dwDatalen);
	if (lRC != ERROR_SUCCESS)
	{
		RegCloseKey(hExtension);
		RegCloseKey(hClasses);
		return (-1);
	}
	Data[dwDatalen] = 0;

	RegCloseKey(hExtension);

	/*--------------------------------------------------------------------
	 * now open the actual DDE entry
	 */
	lRC = RegOpenKeyEx(hClasses, Data, 0, KEY_READ, &hExtension);
	if (lRC != ERROR_SUCCESS)
	{
		RegCloseKey(hClasses);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * now open the command key
	 */
	lRC = RegOpenKeyEx(hExtension, "shell\\open\\command", 0, KEY_READ,
		&hCommand);
	if (lRC != ERROR_SUCCESS)
	{
		RegCloseKey(hExtension);
		RegCloseKey(hClasses);
		return (-1);
	}

	/*--------------------------------------------------------------------
	 * now read in the cmd entry
	 */
	dwDatalen = sizeof(Data);
	lRC = RegQueryValueEx(hCommand, NULL, NULL, &dwType, Data, &dwDatalen);

	RegCloseKey(hCommand);
	RegCloseKey(hExtension);
	RegCloseKey(hClasses);

	if (lRC != ERROR_SUCCESS)
	{
		return (-1);
	}

	Data[dwDatalen] = 0;

	/*--------------------------------------------------------------------
	 * we have the command, so expand it
	 */
	os_dde_cmd_fill(path, Data, cmd);

	return (0);
}

#else

int
os_dde_cmd_get (const char *path, char *cmd)
{
	return (-1);
}

#endif
