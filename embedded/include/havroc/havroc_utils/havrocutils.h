#ifndef HAVROCUTILS_H
#define HAVROCUTILS_H

// common interface includes
#include "uart_if.h"
#include "common.h"

// driverlib includes
#include "utils.h"

#define delay(ms) UtilsDelay((80000/5)*ms)

unsigned long millis ();

#endif /* HAVROCUTILS_H */
