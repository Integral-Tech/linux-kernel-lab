#ifndef CLEANUP_H
#define CLEANUP_H

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

static inline void fd_close(int *fd)
{
	if (*fd >= 0)
		close(*fd);
}

static inline void file_close(FILE **fp)
{
	fclose(*fp);
}

static inline void free_ptr(void *ptr)
{
	free(*(void **)ptr);
}

#endif
