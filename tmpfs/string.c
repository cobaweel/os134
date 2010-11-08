/* OS134, Copyright (C) 2005, Benjamin Stein, Jaap Weel, Ting Liao --
This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or (at
your option) any later version.  This program is distributed in the
hope that it will be useful, but WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the GNU General Public License for more details.  You
should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation,
Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA */

/*
 * Entirely abysmal instance of string.h/string.c. No support for
 * anything except strcmp, since it is used in the filesystem.
 * This needs to be written at a later date, but it is included
 * seperately so that the filesystem will still work without
 * having strcmp internal to it (which would then have to be changed
 * if it was ever implemented in a better manner.
 *
 * It should, in theory, use the protected arrays, but I don't feel
 * like dealing with that right now.
 */

char toupper(char c)
{
	if (( c > 'a' ) && ( c < 'z'))
		return c + 'A' - 'a';
	else
		return c
}

char tolower(char c)
{
	if (( c > 'A' ) && ( c < 'Z'))
		return c + 'a' - 'A';
	else
		return c
}


char * strcpy(char * dest,const char *src)
{
	char *tmp = dest;

	while ((*dest++ = *src++) != '\0')
		/* nothing */;
	return tmp;
}


char * strcat(char * dest, const char * src)
{
	char *tmp = dest;

	while (*dest)
		dest++;
	while ((*dest++ = *src++) != '\0')
		;

	return tmp;
}


int strcmp(const char * cs,const char * ct)
{
	register signed char __res;

	while (1) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
	}

	return __res;
}

int strncmp(const char * cs,const char * ct,int count)
{
	register signed char __res = 0;

	while (count) {
		if ((__res = *cs - *ct++) != 0 || !*cs++)
			break;
		count--;
	}

	return __res;
}
