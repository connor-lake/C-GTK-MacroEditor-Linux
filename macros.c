#include "headers.h"

int msleep(long m) {
    struct timespec rem;
    struct timespec request = {
        (int)(m/1000),
        (m%1000)*1000000
    };
    return nanosleep(&request,&rem);
}


struct event_device {
    char* path;
    char* name;
    char* fname;
    int fd;
};

void create_macro(void **data) {

    GtkDropDown *drop = (GtkDropDown*)data[0];
    struct event_device *devices = (struct event_device*)data[1];
    // GtkWidget *window = data[2]; -- this will be needed when an AlertDialog is used
    g_print("Begin Reading User Inputs from %s\n", devices[gtk_drop_down_get_selected(drop)].path);

    int fd = open(devices[gtk_drop_down_get_selected(drop)].path, O_RDONLY);
    if (fd < 0) { 
        perror("Cannot Open Target Device");
        exit(EXIT_FAILURE);
    }
    
    g_print("Press Button on device %s you wish to be simulated\n", devices[gtk_drop_down_get_selected(drop)].path);

    struct input_event input;
    int code = -1;
    while(code == -1) { 
        read(fd, &input, sizeof(input));
        if(input.type == EV_KEY) {
            code = input.code;
        }
    }

    g_print("KeyCode set: %d\n", code);
    /**
     * We need an AlertDialog here, it will look nicer
     * for now ill just use inputs from stdin
     * we need to figure out a way to create an alert dialog
     * on this thread, because it just doesn't work as the GTK
     * UI is frozen.
     *
     * For AlertDialog -- ]
     * Say something like "reading inputs from device"
     * Close dialog once input found
     * New dialog, capture integer for ms delay
    */
    
    float delay;
    printf("Input Delay in ms: ");
    scanf("%f", &delay);
    printf("Now Simulating Code %d on %s every %fms (CTRL+C to exit)\n", code, devices[gtk_drop_down_get_selected(drop)].path, delay);  
    struct libevdev *dev = NULL;
    int err;
    fd = open(devices[gtk_drop_down_get_selected(drop)].path, O_RDWR|O_NONBLOCK);
    struct libevdev_uinput *uidev;

    libevdev_new_from_fd(fd, &dev);

    err = libevdev_uinput_create_from_device(dev, LIBEVDEV_UINPUT_OPEN_MANAGED, &uidev);
    if (err != 0) {
        perror("Could not create uinput with libevdev");
    }
    while (true) {
        libevdev_uinput_write_event(uidev, EV_KEY, code, 1);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        // msleep(100);
        libevdev_uinput_write_event(uidev, EV_KEY, code, 0);
        libevdev_uinput_write_event(uidev, EV_SYN, SYN_REPORT, 0);
        msleep(delay);
    }


}

static void init_window(GtkApplication *app) {
    // Setup for finding devices
    struct dirent *entry = NULL;
    DIR *dir = opendir("/dev/input");
    if (dir == NULL) {
        perror("/dev/input returned NULL");
    }
    struct event_device *devices = malloc(sizeof(struct event_device) * 512);
    int i = 0;
    while ((entry = readdir(dir)) != NULL) {
        if((entry->d_name)[0] == 'e') { // valid input devices are eventX
            devices[i].path = malloc(strlen("/dev/input/") + strlen(entry->d_name) + 1);
            strcpy(devices[i].path, "/dev/input/");
            strcat(devices[i].path, entry->d_name);


            int fd = open(devices[i].path, O_RDONLY);
            devices[i].fd = fd;
            if (fd < 0) { 
                perror("Cannot Open Devices (are you running as root?)");
            }
            devices[i].name = malloc(512);
            ioctl(fd, EVIOCGNAME(512), devices[i].name);
            
            devices[i].fname = malloc(strlen(" ()") + strlen(devices[i].path) + strlen(devices[i].name) + 1);
            sprintf(devices[i].fname, "%s (%s)", devices[i].path, devices[i].name);
            printf("%s | %s\n", devices[i].path, devices[i].fname);
            i++;
        }
    }
    const char *fdevices[512] = {0};
    for (int k = 0; k < i; k++) {
        fdevices[k] = devices[k].fname;
    }

    closedir(dir);

    GtkWidget *window = gtk_application_window_new(app);
    GtkWidget *grid = gtk_grid_new();
    GtkWidget *label = gtk_label_new("GTK Port python evdev macro editor");

    gtk_window_set_child(GTK_WINDOW(window), grid);
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 400);
    gtk_grid_set_column_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_row_spacing(GTK_GRID(grid), 10);
    gtk_grid_set_column_homogeneous(GTK_GRID(grid), TRUE);

    GtkWidget *create_macro_btn = gtk_button_new_with_label("Create Macro");
    GtkWidget *manage_macros_btn = gtk_button_new_with_label("View/Manage Macros");  

    GtkWidget *deviceslist = gtk_drop_down_new_from_strings(fdevices);
    gtk_grid_attach(GTK_GRID(grid), label, 0, 0, 2, 1);
    gtk_grid_attach(GTK_GRID(grid), deviceslist, 0,1,2,1);
    gtk_grid_attach(GTK_GRID(grid), create_macro_btn, 0,2,1,1);
    gtk_grid_attach(GTK_GRID(grid), manage_macros_btn, 1,2,1,1);

    
    void **data = malloc(sizeof(void*) * 2);
    data[0] = deviceslist;
    data[1] = devices;
    // data[2] = window; -- this will be needed when an alert dialog is used.
    g_signal_connect_swapped(create_macro_btn, "clicked", (GCallback) create_macro, data);
    gtk_window_present(GTK_WINDOW(window));
}


int main(int argc, char* argv[]) {    
    g_autoptr(GtkApplication) app = gtk_application_new(NULL, G_APPLICATION_DEFAULT_FLAGS);
    g_signal_connect(app, "activate", G_CALLBACK(init_window), NULL);
    return g_application_run(G_APPLICATION(app), argc, argv);
}
