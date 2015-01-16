#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_tivac_2_10_01_38/packages;C:/ti/tirtos_tivac_2_10_01_38/products/bios_6_41_00_26/packages;C:/ti/tirtos_tivac_2_10_01_38/products/ndk_2_24_01_18/packages;C:/ti/tirtos_tivac_2_10_01_38/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;H:/Users/Akram/Programs/GitHub/havroc/projects/tiva_tm4c123gh6pm/.config
override XDCROOT = c:/ti/xdctools_3_30_04_52_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_tivac_2_10_01_38/packages;C:/ti/tirtos_tivac_2_10_01_38/products/bios_6_41_00_26/packages;C:/ti/tirtos_tivac_2_10_01_38/products/ndk_2_24_01_18/packages;C:/ti/tirtos_tivac_2_10_01_38/products/uia_2_00_02_39/packages;C:/ti/ccsv6/ccs_base;H:/Users/Akram/Programs/GitHub/havroc/projects/tiva_tm4c123gh6pm/.config;c:/ti/xdctools_3_30_04_52_core/packages;..
HOSTOS = Windows
endif
