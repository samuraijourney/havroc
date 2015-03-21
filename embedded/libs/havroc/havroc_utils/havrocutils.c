/*
 * havrocutils.c
 *
 *  Created on: Mar 20, 2015
 *      Author: Akram
 */

/* BIOS Header files */
#include <xdc/runtime/Timestamp.h>
#include <xdc/runtime/Types.h>
#include <ti/sysbios/knl/Clock.h>

/* HaVRoc Library Includes */
#include <havroc/havroc_utils/havrocutils.h>

unsigned long millis ()
{
	Types_FreqHz freq;

	Timestamp_getFreq(&freq);

	return (Timestamp_get32()*1000.0/(1.0*freq.lo));
}


