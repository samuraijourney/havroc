#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = C:/ti/tirtos_simplelink_2_01_00_03/packages;C:/ti/tirtos_simplelink_2_01_00_03/products/bios_6_40_03_39/packages;C:/ti/tirtos_simplelink_2_01_00_03/products/uia_2_00_01_34/packages;C:/ti/ccsv6/ccs_base
override XDCROOT = c:/ti/xdctools_3_30_05_60_core
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = C:/ti/tirtos_simplelink_2_01_00_03/packages;C:/ti/tirtos_simplelink_2_01_00_03/products/bios_6_40_03_39/packages;C:/ti/tirtos_simplelink_2_01_00_03/products/uia_2_00_01_34/packages;C:/ti/ccsv6/ccs_base;c:/ti/xdctools_3_30_05_60_core/packages;..
HOSTOS = Windows
endif
