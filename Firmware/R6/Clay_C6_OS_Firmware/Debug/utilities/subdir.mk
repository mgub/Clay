################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../utilities/fsl_debug_console.c \
../utilities/fsl_notifier.c \
../utilities/fsl_sbrk.c 

OBJS += \
./utilities/fsl_debug_console.o \
./utilities/fsl_notifier.o \
./utilities/fsl_sbrk.o 

C_DEPS += \
./utilities/fsl_debug_console.d \
./utilities/fsl_notifier.d \
./utilities/fsl_sbrk.d 


# Each subdirectory must supply rules for building sources it contributes
utilities/%.o: ../utilities/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16 -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MK64FN1M0VDC12" -I../Clay/Driver/Button -I../Clay/Driver/Buzzer -I../Clay/Driver/Clock -I../Clay/Driver/GPIO -I../Clay/Driver/I2C -I../Clay/Driver/IMU -I../Clay/Driver/IMU/libmplmpu/core/driver/eMPL -I../Clay/Driver/IMU/libmplmpu/core/driver/include -I../Clay/Driver/IMU/libmplmpu/core/eMPL-hal -I../Clay/Driver/IMU/libmplmpu/core/mllite -I../Clay/Driver/IMU/libmplmpu/core/mpl -I../Clay/Driver/LEDs -I../Clay/Driver/Mesh -I../Clay/Driver/Mesh/RadioHead -I../Clay/Driver/Power_Manager -I../Clay/Driver/RGB_LED -I../Clay/Driver/WiFi -I../Clay/Application -I../Clay/Bootloader -I../Clay/Message -I../Clay/Utilities -I../freertos/Source/include -I../startup -I../board -I../utilities -I../freertos/Source/portable/GCC/ARM_CM4F -I../CMSIS -I../source -I../drivers -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


