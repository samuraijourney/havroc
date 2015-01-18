#if defined(CCWARE) || defined(TIVAWARE)
	#include <xdc/std.h>
	#include <xdc/runtime/System.h>
	#include <ti/sysbios/BIOS.h>
#endif

#include <havroc/tracking/service.h>

Int main()
{
    System_printf("Enter main()\n");
	System_flush();

    if(!start_service())
    {
    	System_printf("Service start success\n");
    }
    else
    {
    	System_printf("Service start error\n");
    }
	System_flush();

    BIOS_start();
    return(0);
}
