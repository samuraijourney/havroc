#if defined(cc3200) || defined(TIVAWARE)
	#include <xdc/std.h>
	#include <xdc/runtime/System.h>
	#include <ti/sysbios/BIOS.h>
#endif

Int main()
{ 
    System_printf("Hello world!\n");
	System_flush();

    BIOS_start();
    return(0);
}
