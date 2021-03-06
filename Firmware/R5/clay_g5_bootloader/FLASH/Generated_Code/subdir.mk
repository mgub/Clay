################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Generated_Code/Cpu.c" \
"../Generated_Code/FLASH1.c" \
"../Generated_Code/LED1.c" \
"../Generated_Code/LED2.c" \
"../Generated_Code/PE_LDD.c" \
"../Generated_Code/TU1.c" \
"../Generated_Code/Vectors.c" \
"../Generated_Code/tick_1ms_timer.c" \

C_SRCS += \
../Generated_Code/Cpu.c \
../Generated_Code/FLASH1.c \
../Generated_Code/LED1.c \
../Generated_Code/LED2.c \
../Generated_Code/PE_LDD.c \
../Generated_Code/TU1.c \
../Generated_Code/Vectors.c \
../Generated_Code/tick_1ms_timer.c \

OBJS += \
./Generated_Code/Cpu.o \
./Generated_Code/FLASH1.o \
./Generated_Code/LED1.o \
./Generated_Code/LED2.o \
./Generated_Code/PE_LDD.o \
./Generated_Code/TU1.o \
./Generated_Code/Vectors.o \
./Generated_Code/tick_1ms_timer.o \

C_DEPS += \
./Generated_Code/Cpu.d \
./Generated_Code/FLASH1.d \
./Generated_Code/LED1.d \
./Generated_Code/LED2.d \
./Generated_Code/PE_LDD.d \
./Generated_Code/TU1.d \
./Generated_Code/Vectors.d \
./Generated_Code/tick_1ms_timer.d \

OBJS_QUOTED += \
"./Generated_Code/Cpu.o" \
"./Generated_Code/FLASH1.o" \
"./Generated_Code/LED1.o" \
"./Generated_Code/LED2.o" \
"./Generated_Code/PE_LDD.o" \
"./Generated_Code/TU1.o" \
"./Generated_Code/Vectors.o" \
"./Generated_Code/tick_1ms_timer.o" \

C_DEPS_QUOTED += \
"./Generated_Code/Cpu.d" \
"./Generated_Code/FLASH1.d" \
"./Generated_Code/LED1.d" \
"./Generated_Code/LED2.d" \
"./Generated_Code/PE_LDD.d" \
"./Generated_Code/TU1.d" \
"./Generated_Code/Vectors.d" \
"./Generated_Code/tick_1ms_timer.d" \

OBJS_OS_FORMAT += \
./Generated_Code/Cpu.o \
./Generated_Code/FLASH1.o \
./Generated_Code/LED1.o \
./Generated_Code/LED2.o \
./Generated_Code/PE_LDD.o \
./Generated_Code/TU1.o \
./Generated_Code/Vectors.o \
./Generated_Code/tick_1ms_timer.o \


# Each subdirectory must supply rules for building sources it contributes
Generated_Code/Cpu.o: ../Generated_Code/Cpu.c
	@echo 'Building file: $<'
	@echo 'Executing target #8 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Cpu.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Cpu.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/FLASH1.o: ../Generated_Code/FLASH1.c
	@echo 'Building file: $<'
	@echo 'Executing target #9 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/FLASH1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/FLASH1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/LED1.o: ../Generated_Code/LED1.c
	@echo 'Building file: $<'
	@echo 'Executing target #10 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/LED1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/LED1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/LED2.o: ../Generated_Code/LED2.c
	@echo 'Building file: $<'
	@echo 'Executing target #11 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/LED2.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/LED2.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/PE_LDD.o: ../Generated_Code/PE_LDD.c
	@echo 'Building file: $<'
	@echo 'Executing target #12 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/PE_LDD.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/PE_LDD.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/TU1.o: ../Generated_Code/TU1.c
	@echo 'Building file: $<'
	@echo 'Executing target #13 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/TU1.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/TU1.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/Vectors.o: ../Generated_Code/Vectors.c
	@echo 'Building file: $<'
	@echo 'Executing target #14 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/Vectors.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/Vectors.o"
	@echo 'Finished building: $<'
	@echo ' '

Generated_Code/tick_1ms_timer.o: ../Generated_Code/tick_1ms_timer.c
	@echo 'Building file: $<'
	@echo 'Executing target #15 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Generated_Code/tick_1ms_timer.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Generated_Code/tick_1ms_timer.o"
	@echo 'Finished building: $<'
	@echo ' '


