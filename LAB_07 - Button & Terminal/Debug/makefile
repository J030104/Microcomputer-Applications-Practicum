################################################################################
# 自?生成的文件。不要??！
################################################################################

ifndef ANDESIGHT_ROOT
ANDESIGHT_ROOT=/cygdrive/C/Andestech/AndeSight211MCU
endif

ifndef CROSS_COMPILE
CROSS_COMPILE=nds32le-elf-
endif

ifndef SECONDARY_OUTPUT_PATH
SECONDARY_OUTPUT_PATH=output
endif

$(shell mkdir -p $(SECONDARY_OUTPUT_PATH))

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 
BUILD_ARTIFACT_FILE_BASE_NAME = WT58F2C9_UART

LINKER_OUTPUTS = WT58F2C9_UART.adx

GCOV_OUT += \
$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)) \

GPROF_OUT += \
../gmon.sum ../gmon.out \

SYMBOL_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/symbol.txt \

READELF_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/readelf.txt \

OBJDUMP_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/objdump.txt \

OBJCOPY_OUTPUTS += \
$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin \

TOOL_NDS_SIZE_OUTPUT_OUTPUTS += \
.PHONY.size \


# 所有目?
all: WT58F2C9_UART.adx secondary-outputs

# 工具?用
WT58F2C9_UART.adx: $(OBJS) $(USER_OBJS)
	@echo '正在构建目?： $@'
	@echo '正在?用： Andes C Linker'
	$(CROSS_COMPILE)gcc -O0 -nostartfiles -static -mcmodel=medium -T"../nds32-c-ext.ld" -o "WT58F2C9_UART.adx" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo '已?束构建目?： $@'
	@echo ' '

$(foreach subdir,$(SUBDIRS),$(wildcard $(subdir)/*.gcda) $(wildcard $(subdir)/*.gcno)): WT58F2C9_UART.adx $(OBJS) $(USER_OBJS)
../gmon.sum ../gmon.out: WT58F2C9_UART.adx $(OBJS) $(USER_OBJS)

$(SECONDARY_OUTPUT_PATH)/symbol.txt: $(LINKER_OUTPUTS)
	@echo '正在?用： NM (symbol listing)'
	$(CROSS_COMPILE)nm -n -l -C "WT58F2C9_UART.adx" > $(SECONDARY_OUTPUT_PATH)/symbol.txt
	@echo '已?束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/readelf.txt: $(LINKER_OUTPUTS)
	@echo '正在?用： Readelf (ELF info listing)'
	$(CROSS_COMPILE)readelf -a "WT58F2C9_UART.adx" > $(SECONDARY_OUTPUT_PATH)/readelf.txt
	@echo '已?束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/objdump.txt: $(LINKER_OUTPUTS)
	@echo '正在?用： Objdump (disassembly)'
	$(CROSS_COMPILE)objdump -x -d -C "WT58F2C9_UART.adx" > $(SECONDARY_OUTPUT_PATH)/objdump.txt
	@echo '已?束构建： $@'
	@echo ' '

$(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin: $(LINKER_OUTPUTS)
	@echo '正在?用： Objcopy (object content copy)'
	$(CROSS_COMPILE)objcopy -S -O binary "WT58F2C9_UART.adx" $(SECONDARY_OUTPUT_PATH)/$(BUILD_ARTIFACT_FILE_BASE_NAME).bin
	@echo '已?束构建： $@'
	@echo ' '

.PHONY.size: $(LINKER_OUTPUTS)
	@echo '正在?用： Size (section size listing)'
	$(CROSS_COMPILE)size  "WT58F2C9_UART.adx"
	@echo '已?束构建： $@'
	@echo ' '

# 其他目?
clean:
	-$(RM) $(TOOL_NDS_SIZE_OUTPUT_OUTPUTS)$(OBJCOPY_OUTPUTS)$(OBJS)$(C_DEPS)$(GCOV_OUT)$(SYMBOL_OUTPUTS)$(OBJDUMP_OUTPUTS)$(READELF_OUTPUTS)$(GPROF_OUT)$(EXECUTABLES)$(S_UPPER_DEPS) WT58F2C9_UART.adx
	-@echo ' '

secondary-outputs: $(SYMBOL_OUTPUTS) $(READELF_OUTPUTS) $(OBJDUMP_OUTPUTS) $(OBJCOPY_OUTPUTS) $(TOOL_NDS_SIZE_OUTPUT_OUTPUTS)

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
