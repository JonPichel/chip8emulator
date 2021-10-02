#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>

int main(void) {
    char rpath[PATH_MAX + 1];
    realpath("./../roms", rpath);
    printf("%s, %d\n", rpath, opendir(rpath));
}
