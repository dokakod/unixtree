/*------------------------------------------------------------------------
 * curses utility routines
 */
#include "wincommon.h"

char * keyname (int c)
{
	return (char *)tcap_get_key_name(c, 0);
}

int get_key_value (const char *name)
{
	return tcap_get_key_value(name);
}

const char * get_key_name (int code, char *buf)
{
	return tcap_get_key_name(code, buf);
}

const char * get_char_name (int code, char *buf)
{
	return tcap_get_char_name(code, buf);
}

const char * get_attr_name_by_code (attr_t code, char *buf)
{
	return tcap_get_attr_name_by_code(code, buf);
}

const char * get_color_name_by_code (attr_t code, char *buf)
{
	return tcap_get_color_name_by_code(code, buf);
}

const char * get_fg_name_by_code (attr_t code)
{
	return tcap_get_fg_name_by_code(code);
}

const char * get_bg_name_by_code (attr_t code)
{
	return tcap_get_bg_name_by_code(code);
}

const char * get_attr_name_by_num (int n)
{
	return tcap_get_attr_name_by_num(n);
}

const char * get_fg_name_by_num (int n)
{
	return tcap_get_fg_name_by_num(n);
}

const char * get_bg_name_by_num (int n)
{
	return tcap_get_bg_name_by_num(n);
}

attr_t get_attr_value_by_num (int n)
{
	return tcap_get_attr_value_by_num(n);
}

attr_t get_fg_value_by_num (int n)
{
	return tcap_get_fg_value_by_num(n);
}

attr_t get_bg_value_by_num (int n)
{
	return tcap_get_bg_value_by_num(n);
}

int get_attr_num_by_name (const char *name)
{
	return tcap_get_attr_num_by_name(name);
}

int get_fg_num_by_name (const char *name)
{
	return tcap_get_fg_num_by_name(name);
}

int get_bg_num_by_code (attr_t code)
{
	return tcap_get_bg_num_by_code(code);
}

int get_attr_num_by_code (attr_t code)
{
	return tcap_get_attr_num_by_code(code);
}

int get_fg_num_by_code (attr_t code)
{
	return tcap_get_fg_num_by_code(code);
}

int get_bg_num_by_name (const char *name)
{
	return tcap_get_bg_num_by_name(name);
}

unsigned short get_2byte (FILE *fp)
{
	return tcap_get_2byte(fp);
}

int put_2byte (FILE *fp, unsigned short s)
{
	return tcap_put_2byte(fp, s);
}

unsigned int get_4byte (FILE *fp)
{
	return tcap_get_4byte(fp);
}

int put_4byte (FILE *fp, unsigned int l)
{
	return tcap_put_4byte(fp, l);
}

time_t get_4time (FILE *fp)
{
	return tcap_get_4time(fp);
}

int put_4time (FILE *fp, time_t t)
{
	return tcap_put_4time(fp, t);
}
