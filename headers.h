#include <gtk/gtk.h>
#include <termios.h>
#include <linux/input.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <libevdev/libevdev.h>
#include <libevdev/libevdev-uinput.h>
#include <time.h>
#include <stdbool.h>

int msleep(long m) {
    struct timespec rem;
    struct timespec request = {
        (int)(m/1000),
        (m%1000)*1000000
    };
    return nanosleep(&request,&rem);
}


