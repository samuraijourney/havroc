################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
configPkg/linker.cmd: ../empty.cfg
	@echo 'Building file: $<'
	@echo 'Invoking: XDCtools'
	"c:/ti/xdctools_3_30_05_60_core/xs" --xdcpath="c:/ti/tirtos_simplelink_2_01_00_03/packages;c:/ti/tirtos_simplelink_2_01_00_03/products/bios_6_40_03_39/packages;c:/ti/tirtos_simplelink_2_01_00_03/products/uia_2_00_01_34/packages;c:/ti/ccsv6/ccs_base;" xdc.tools.configuro -o configPkg -t ti.targets.arm.elf.M4 -p ti.platforms.simplelink:CC3200 -r release -c "c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0" "$<"
	@echo 'Finished building: $<'
	@echo ' '

configPkg/compiler.opt: | configPkg/linker.cmd
configPkg/: | configPkg/linker.cmd


