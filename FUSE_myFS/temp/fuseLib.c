#include "fuseLib.h"

#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>
#include <inttypes.h>
#include <linux/kdev_t.h>


//function to  remove (unlink) a file 

static int my_unlink(const char *path) {

	// Based of my_mknod()

	// Given the path, we need to traverse it as a FAT, and then work out from there

	fprintf(stderr, "--->>>my_unlink: path %s", path);

	// We check that the length of the file name is correct
	if(st