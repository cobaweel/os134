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
 * FS driver.
 */

#include "fs.h"

/*
 * Mounts a file system, i.e. gives you a dir_handle to its root
 * directory. For the time being, takes a UNIX file name that
 * describes a disk image file.
 */
fs_error mount(char* imagefile, dir_handle* root){
     /* ***TODO*** */

     return FS_OK;
}

/*
 * Unmounts a filesystem or closes a directory handle.
 */
fs_error closedir(dir_handle root){
     /* ***TODO*** */

     return FS_OK;
}

/*
 * Opens a directory. Even if directories happen to be implemented as
 * files, they should only be accessible through this function, and
 * not through open/close. 
 */
fs_error getdir(dir_handle parent, dir_handle* child, char* name){
     /* ***TODO*** */

     return FS_OK;
}

/* 
 * Return the name of the next file in the directory. Throws EOD.
 */
fs_error nextname(dir_handle dir, char* buf){
     /* ***TODO*** */

     return FS_OK;
}

/*
 * Resets the pointer for a directory to point at the n-th
 * file. Throws EOD.
 */
fs_error resetd(dir_handle dir, unsigned n){
     /* ***TODO*** */

     return FS_OK;
}

/*
 * Create a new, empty file in directory.
 */
fs_error create(dir_handle dir, char* name){
     /* ***TODO*** */

     return FS_OK;
}

/*
 * Delete a file. (No hard/sym links so far, so no reference counts or
 * any of that).
 */
fs_error delete(dir_handle dir, char* name){
     /* ***TODO*** */

     return FS_OK;
}

/*
 * Opens file, returning the resulting handle by reference and an
 * error code by value. Acquire an exclusive lock on the file. We
 * shall (for now) be very unsophisticated about that. The file
 * initially has the RW pointer point at its beginning.
 */
 fs_error open(dir_handle dir, file_handle* file, char* name){
/* ***TODO*** */

return FS_OK;
}

/* 
 * Closes a file. Releases the lock. 
 */
fs_error close(file_handle file){
/* ***TODO*** */

return FS_OK;
}

/*
 * Read n characters starting at the RW pointer, and advance the RW
 * pointer accordingly. Throws EOF.
 */
fs_error readn(char* buf, unsigned n, file_handle file){
/* ***TODO*** */

return FS_OK;
}

/*
 * Write n characters starting at the RW pointer, and advance the RW
 * pointer accordingly. Never throw EOF, just expand file.
 */
fs_error writen(char* buf, unsigned n, file_handle file){
/* ***TODO*** */

return FS_OK;
}

/* 
 * Reset the RW pointer to byte n. Throws EOF.
 */
fs_error resetf(file_handle file, unsigned n){
/* ***TODO*** */

return FS_OK;
}
