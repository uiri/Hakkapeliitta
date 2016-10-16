#ifndef EPIPHANY_H_
#define EPIPHANY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <e-hal.h>
#include <e-loader.h>

e_epiphany_t *init_epiphany_threadpool(void);
void cleanup_epiphany_threadpool(e_epiphany_t *dev);

#ifdef __cplusplus
}
#endif

#endif
