/*------------------------------------------------------------------------
 * OS readlink mechanism
 */
#include "oscommon.h"

#if V_WINDOWS

#include <shlobj.h>

extern "C" int os_win_readlink (const char *linkname, char *pathname)
{
	HRESULT			hres;
	WORD			wsz[MAX_PATH];
	WIN32_FIND_DATA	wfd;
	IShellLink *	psl;
	IPersistFile *	ppf;

	//--------------------------------------------------------------------
	// initialize the COM interface
	//
	CoInitialize(0);

	//--------------------------------------------------------------------
	// initialize the IShellLink object
	//
	hres = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&psl);
	if (! SUCCEEDED(hres))
	{
		CoUninitialize();
		return (-1);
	}

	//--------------------------------------------------------------------
	// initialize the IPersistFile object
	//
	hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
	if (! SUCCEEDED(hres))
	{
		psl->Release();
		CoUninitialize();
		return (-1);
	}

	//--------------------------------------------------------------------
	// convert linkname to multi-byte
	//
	MultiByteToWideChar(CP_ACP, 0, linkname, -1, wsz, sizeof(wsz));

	//--------------------------------------------------------------------
	// now load the shell link
	//
	hres = ppf->Load(wsz, STGM_READ);
	if (! SUCCEEDED(hres))
	{
		ppf->Release();
		psl->Release();
		CoUninitialize();
		return (-1);
	}

	//--------------------------------------------------------------------
	// now get the pathname
	//
	hres = psl->GetPath(pathname, MAX_PATH, &wfd, SLGP_UNCPRIORITY);

	//--------------------------------------------------------------------
	// release the objects
	//
	ppf->Release();
	psl->Release();

	//--------------------------------------------------------------------
	// close out the com interface
	//
	CoUninitialize();

	return ( SUCCEEDED(hres) ? 0 : -1 );
}

extern "C" int os_win_makelink (const char *linkname, const char *pathname)
{
	HRESULT			hres;
	WORD			wsz[MAX_PATH];
	IShellLink *	psl;
	IPersistFile *	ppf;

	//--------------------------------------------------------------------
	// initialize the COM interface
	//
	CoInitialize(0);

	//--------------------------------------------------------------------
	// initialize the IShellLink object
	//
	hres = CoCreateInstance(CLSID_ShellLink, 0, CLSCTX_INPROC_SERVER,
		IID_IShellLink, (void **)&psl);
	if (! SUCCEEDED(hres))
	{
		CoUninitialize();
		return (-1);
	}

	//--------------------------------------------------------------------
	// initialize the IPersistFile object
	//
	hres = psl->QueryInterface(IID_IPersistFile, (void **)&ppf);
	if (! SUCCEEDED(hres))
	{
		psl->Release();
		CoUninitialize();
		return (-1);
	}

	//--------------------------------------------------------------------
	// now set the path to the original file
	//
	hres = psl->SetPath(pathname);
	if (! SUCCEEDED(hres))
	{
		ppf->Release();
		psl->Release();
		CoUninitialize();
		return (-1);
	}

	//--------------------------------------------------------------------
	// convert linkname to multi-byte
	//
	MultiByteToWideChar(CP_ACP, 0, linkname, -1, wsz, sizeof(wsz));

	//--------------------------------------------------------------------
	// now get the pathname
	//
	hres = ppf->Save(wsz, TRUE);

	//--------------------------------------------------------------------
	// release the objects
	//
	ppf->Release();
	psl->Release();

	//--------------------------------------------------------------------
	// close out the com interface
	//
	CoUninitialize();

	return ( SUCCEEDED(hres) ? 0 : -1 );
}

#else

int os_win_readlink (const char *linkname, char *pathname)
{
	return (-1);
}

int os_win_makelink (const char *linkname, const char *pathname)
{
	return (-1);
}

#endif
