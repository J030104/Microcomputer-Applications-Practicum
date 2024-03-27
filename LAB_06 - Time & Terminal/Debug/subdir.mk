################################################################################
# 自?生成的文件。不要??！
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../init-default.c \
../init-soc.c \
../interrupt-c-ext.c \
../main.c \
../uart.c 

OBJS += \
./init-default.o \
./init-soc.o \
./interrupt-c-ext.o \
./main.o \
./uart.o 

C_DEPS += \
./init-default.d \
./init-soc.d \
./interrupt-c-ext.d \
./main.d \
./uart.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo '正在构建文件： $<'
	@echo '正在?用： Andes C Compiler'
	$(CROSS_COMPILE)gcc -DREMAP -DUSE_C_EXT -O0 -mcmodel=medium -g3 -Wall -mcpu=n801 -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo '已?束构建： $<'
	@echo ' '


