################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
common/CC3200_LP.obj: ../common/CC3200_LP.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/bin/armcl" -mv7M4 --code_state=16 --abi=eabi -me --include_path="c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/include" --include_path="H:/Users/Akram/Programs/GitHub/havroc/embedded/include/havroc" --include_path="H:/Users/Akram/Programs/GitHub/havroc/embedded/include/apps_universal" --include_path="H:/Users/Akram/Programs/GitHub/havroc/embedded/include/apps_cc3200" --include_path="c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2" --include_path="c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2/inc" --include_path="c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2/driverlib" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --preproc_with_compile --preproc_dependency="common/CC3200_LP.pp" --obj_directory="common" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

common/empty_rom_pin_mux_config.obj: ../common/empty_rom_pin_mux_config.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/bin/armcl" -mv7M4 --code_state=16 --abi=eabi -me --include_path="c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/include" --include_path="H:/Users/Akram/Programs/GitHub/havroc/embedded/include/havroc" --include_path="H:/Users/Akram/Programs/GitHub/havroc/embedded/include/apps_universal" --include_path="H:/Users/Akram/Programs/GitHub/havroc/embedded/include/apps_cc3200" --include_path="c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2" --include_path="c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2/inc" --include_path="c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2/driverlib" -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off --preproc_with_compile --preproc_dependency="common/empty_rom_pin_mux_config.pp" --obj_directory="common" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


