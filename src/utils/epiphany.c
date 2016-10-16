#include <stdlib.h>
#include "epiphany.h"

e_epiphany_t *init_epiphany_threadpool(void) {
  e_platform_t platform;
  e_epiphany_t *dev;
  unsigned int i, j;
  int done;

  dev = (e_epiphany_t*)malloc(sizeof(e_epiphany_t));

  // e_set_loader_verbosity(H_D0);
  e_set_host_verbosity(H_D0);

  // initialize system, read platform params from
  // default HDF. Then, reset the platform and
  // get the actual system parameters.
  e_init(NULL);
  e_reset_system();
  e_get_platform_info(&platform);
  e_open(dev, 0, 0, platform.rows, platform.cols);

  /* Initialize each core's `done` variable to 1 so that it will accept a job */
  done = 1;
  for (i = 0; i < platform.rows; i++) {
    for (j = 0; j < platform.cols; j++) {
      done = 0;
      e_write(dev, i, j, 0x0, &done, sizeof(done));
    }
  }

  e_load_group("e_task.elf", dev, 0, 0, platform.rows, platform.cols, E_FALSE);

  return dev;
}

void cleanup_epiphany_threadpool(e_epiphany_t *dev) {
  e_close(dev);
  e_finalize();
  free(dev);
}

