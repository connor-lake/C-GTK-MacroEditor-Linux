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

