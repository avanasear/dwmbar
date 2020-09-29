#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <net/if.h>
#include <pthread.h>
#include <X11/Xlib.h>
#include <sys/ioctl.h>
#include <arpa/inet.h>

#define NUM_THREADS 2

int STOPNO = 0;
char DATE[20] = {0};
char IPADDR[16] = {0};

void * datetime (void * args);
void * ipaddr (void * args);

int
main(int argc, char ** argv)
{
    size_t outsize = 38;
    char output[outsize];

    Display * dpy = NULL;
    Window win = 0;
    pthread_t threads[NUM_THREADS] = {0};

    dpy = XOpenDisplay(getenv("DISPLAY"));
    if (dpy == NULL)
    {
        fprintf(stderr, "Can't open $DISPLAY, exiting.\n");
        exit(1);
    }
    win = DefaultRootWindow(dpy);

    pthread_create(&threads[0], NULL, &datetime, NULL);
    pthread_create(&threads[1], NULL, &ipaddr, NULL);

    while (STOPNO == 0)
    {
        snprintf(output, outsize, "%s | %s", IPADDR, DATE);
        XStoreName(dpy, win, output);
        XFlush(dpy);

        if (getenv("DISPLAY") == NULL)
        {
            STOPNO++;
        }
        sleep(1);
    }

    return 0;
}

void *
datetime(void * args)
{
    size_t datesize = 20;
    char datestr[datesize];

    int hr, min, sec, day, mo, yr = 0;
    time_t now;
    struct tm * local;

    while (STOPNO == 0)
    {
        time(&now);
        local = localtime(&now);

        hr = local->tm_hour;
        min = local->tm_min;
        sec = local->tm_sec;

        day = local->tm_mday;
        mo = local->tm_mon + 1;
        yr = local->tm_year + 1900;

        snprintf(datestr, datesize, "%d/%02d/%02d %02d:%02d:%02d\0", yr, mo, day, hr, min, sec);
        strncpy(DATE, datestr, datesize);
        usleep(10000);
    }

    return 0;
}

void *
ipaddr(void * args)
{
    int fd;
    struct ifreq ifr;

    size_t ipsize = 16;
    char ipstr[ipsize];

    fd = socket(AF_INET, SOCK_DGRAM, 0);
    ifr.ifr_addr.sa_family = AF_INET;
    snprintf(ifr.ifr_name, IFNAMSIZ, "wlan0");

    while (STOPNO == 0)
    {
        ioctl(fd, SIOCGIFADDR, &ifr);

        snprintf(ipstr, ipsize, "%s", inet_ntoa(((struct sockaddr_in *)&ifr.ifr_addr)->sin_addr));
        strncpy(IPADDR, ipstr, ipsize);

        sleep(5);
    }

    close(fd);
    return 0;
}
