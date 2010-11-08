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
 * Interface to the FS driver.
 */

#ifndef _FS_H
#define _FS_H

/*
 * Errors in the filesystem.
 */
enum fs_error { 
     FS_OK,                     /* Succesful operation. */
     FS_TROUBLE,                /* General error message. */
     FS_PANIC,                  /* Internal error; hardware failure. */
     FS_INVALID_HANDLE,         /* Invalid file or dir handle. */
     FS_EOF,                    /* End of file. */
     FS_EOD,                    /* End of directory. */
     FS_FULL                    /* Disk full. */
     /* ***TODO*** */
     /* Add any additional FS related errors. */
};
typedef enum fs_error fs_error;

/*
 * Unique handles.
 *
 * Allocate *unique* file handles. When a closed file handle is used,
 * FS_INVALID_HANDLE is thrown by all functions.
 */

/*
 * We allow only one handle for any given file at a time, which is for
 * reading as well as writing.
 */
typedef unsigned file_handle;

/* 
 * Directories have a different API. More than one handle to a
 * directory may exist at a time.
 */
typedef unsigned dir_handle;

/*
 * Mounts a file system, i.e. gives you a dir_handle to its root
 * directory. For the time being, takes a UNIX file name that
 * describes a disk image file.
 */
fs_error mount(char* imagefile, dir_handle* root);

/*
 * Unmounts a filesystem or closes a directory handle.
 */
fs_error closedir(dir_handle root);

/*
 * Opens a directory. Even if directories happen to be implemented as
 * files, they should only be accessible through this function, and
 * not through open/close. 
 */
fs_error getdir(dir_handle parent, dir_handle* child, char* name);

/* 
 * Return the name of the next file in the directory. Throws EOD.
 */
fs_error nextname(dir_handle dir, char* buf);

/*
 * Resets the pointer for a directory to point at the n-th
 * file. Throws EOD.
 */
fs_error resetd(dir_handle dir, unsigned n);

/*
 * Create a new, empty file in directory.
 */
fs_error create(dir_handle dir, char* name);

/*
 * Delete a file. (No hard/sym links so far, so no reference counts or
 * any of that).
 */
fs_error delete(dir_handle dir, char* name);

/*
 * Opens file, returning the resulting handle by reference and an
 * error code by value. Acquire an exclusive lock on the file; we
 * shall (for now) be very unsophisticated about that. The file
 * initially has the RW pointer point at its beginning.
 */
fs_error open(dir_handle dir, file_handle* file, char* name);

/* 
 * Closes a file. Releases the lock. 
 */
fs_error close(file_handle file);

/*
 * Read n characters starting at the RW pointer, and advance the RW
 * pointer accordingly. Throws EOF.
 */
fs_error readn(char* buf, unsigned n, file_handle file);

/*
 * Write n characters starting at the RW pointer, and advance the RW
 * pointer accordingly. Never throw EOF, just expand file.
 */
fs_error writen(char* buf, unsigned n, file_handle file);

/* 
 * Reset the RW pointer to byte n. Throws EOF.
 */
fs_error resetf(file_handle file, unsigned n);

#endif /* _FS_H */
