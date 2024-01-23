#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/inotify.h>
#include <unistd.h>

#define EVENT_SIZE (sizeof(struct inotify_event))

char *get_mode_label(int d)
{
  switch (d)
  {
  case 0:
    return "Normal";
  case 1:
    return "Turbo";
  case 2:
    return "Silence";
  default:
    return "Unkown";
  }
}

int main()
{
  int fd;
  int wd;
  struct inotify_event event;
  int mode;

  fd = inotify_init();
  if (fd < 0)
  {
    perror("inotify_init");
    return 1;
  }

  wd = inotify_add_watch(fd, "/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy", IN_MODIFY);

  while (1)
  {
    read(fd, &event, EVENT_SIZE);
    if (event.mask & IN_MODIFY)
    {
      FILE *f = fopen("/sys/devices/platform/asus-nb-wmi/throttle_thermal_policy", "r");
      if (f == NULL)
      {
        perror("cannot open throttle_thermal_policy file \n");
        return 1;
      }
      fscanf(f, "%d", &mode);
      char command[64];
      sprintf(command, "notify-send \"Fan Mode\" \"%s\"", get_mode_label(mode));
      system(command);
      fclose(f);
    }
  }

  inotify_rm_watch(fd, wd);
  close(fd);
}