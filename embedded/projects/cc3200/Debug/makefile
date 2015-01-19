################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

CG_TOOL_ROOT := c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0

ORDERED_OBJS += \
$(GEN_CMDS__FLAG) \
"./modules/tracking/service.obj" \
"./modules/communications/suit/suit_i2c.obj" \
"./common/common_cc3200/CC3200_LP.obj" \
"./common/common_cc3200/empty_rom_pin_mux_config.obj" \
"./apps/apps_cc3200/empty_example/main.obj" \
"../CC3200_LP.cmd" \
-l"libc.a" \
-l"c:/ti/tirtos_simplelink_2_01_00_03/products/CC3200_driverlib_1.0.2/driverlib/ccs/Release/driverlib.a" \

-include ../makefile.init

RM := DEL /F
RMDIR := RMDIR /S/Q

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir_vars.mk
-include modules/tracking/subdir_vars.mk
-include modules/communications/suit/subdir_vars.mk
-include common/common_cc3200/subdir_vars.mk
-include apps/apps_cc3200/empty_example/subdir_vars.mk
-include subdir_rules.mk
-include modules/tracking/subdir_rules.mk
-include modules/communications/suit/subdir_rules.mk
-include common/common_cc3200/subdir_rules.mk
-include apps/apps_cc3200/empty_example/subdir_rules.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(S62_DEPS)),)
-include $(S62_DEPS)
endif
ifneq ($(strip $(C64_DEPS)),)
-include $(C64_DEPS)
endif
ifneq ($(strip $(ASM_DEPS)),)
-include $(ASM_DEPS)
endif
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(S55_DEPS)),)
-include $(S55_DEPS)
endif
ifneq ($(strip $(C67_DEPS)),)
-include $(C67_DEPS)
endif
ifneq ($(strip $(C??_DEPS)),)
-include $(C??_DEPS)
endif
ifneq ($(strip $(CLA_DEPS)),)
-include $(CLA_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
ifneq ($(strip $(S??_DEPS)),)
-include $(S??_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(C62_DEPS)),)
-include $(C62_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(ASM_UPPER_DEPS)),)
-include $(ASM_UPPER_DEPS)
endif
ifneq ($(strip $(K_DEPS)),)
-include $(K_DEPS)
endif
ifneq ($(strip $(C43_DEPS)),)
-include $(C43_DEPS)
endif
ifneq ($(strip $(S67_DEPS)),)
-include $(S67_DEPS)
endif
ifneq ($(strip $(SA_DEPS)),)
-include $(SA_DEPS)
endif
ifneq ($(strip $(S43_DEPS)),)
-include $(S43_DEPS)
endif
ifneq ($(strip $(OPT_DEPS)),)
-include $(OPT_DEPS)
endif
ifneq ($(strip $(S64_DEPS)),)
-include $(S64_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(C55_DEPS)),)
-include $(C55_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
EXE_OUTPUTS += \
havroc_CC3200.out \

EXE_OUTPUTS__QUOTED += \
"havroc_CC3200.out" \

BIN_OUTPUTS += \
havroc_CC3200.hex \

BIN_OUTPUTS__QUOTED += \
"havroc_CC3200.hex" \


# All Target
all: havroc_CC3200.out

# Tool invocations
havroc_CC3200.out: $(OBJS) $(CMD_SRCS) $(GEN_CMDS)
	@echo 'Building target: $@'
	@echo 'Invoking: ARM Linker'
	"c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/bin/armcl" -mv7M4 --code_state=16 --abi=eabi -me -g --define=cc3200 --define=PART_CC3200 --define=ccs --define=CCWARE --display_error_number --diag_warning=225 --diag_warning=255 --diag_wrap=off -z -m"havroc_CC3200.map" --heap_size=0x0 --stack_size=0x512 -i"c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/lib" -i"c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/include" --reread_libs --warn_sections --display_error_number --diag_wrap=off --xml_link_info="havroc_CC3200_linkInfo.xml" --rom_model -o "havroc_CC3200.out" $(ORDERED_OBJS)
	@echo 'Finished building target: $@'
	@echo ' '

havroc_CC3200.hex: $(EXE_OUTPUTS)
	@echo 'Invoking: ARM Hex Utility'
	"c:/ti/ccsv6/tools/compiler/ti-cgt-arm_5.2.0/bin/armhex"  -o "havroc_CC3200.hex" $(EXE_OUTPUTS__QUOTED)
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(GEN_MISC_FILES__QUOTED)$(EXE_OUTPUTS__QUOTED)$(GEN_FILES__QUOTED)$(BIN_OUTPUTS__QUOTED)$(GEN_OPTS__QUOTED)$(GEN_CMDS__QUOTED)
	-$(RMDIR) $(GEN_MISC_DIRS__QUOTED)
	-$(RM) "modules\tracking\service.pp" "modules\communications\suit\suit_i2c.pp" "common\common_cc3200\CC3200_LP.pp" "common\common_cc3200\empty_rom_pin_mux_config.pp" "apps\apps_cc3200\empty_example\main.pp" 
	-$(RM) "modules\tracking\service.obj" "modules\communications\suit\suit_i2c.obj" "common\common_cc3200\CC3200_LP.obj" "common\common_cc3200\empty_rom_pin_mux_config.obj" "apps\apps_cc3200\empty_example\main.obj" 
	-@echo 'Finished clean'
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets

