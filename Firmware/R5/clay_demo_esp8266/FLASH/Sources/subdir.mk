################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.local

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS_QUOTED += \
"../Sources/Application.c" \
"../Sources/ESP8266.c" \
"../Sources/Events.c" \
"../Sources/main.c" \

C_SRCS += \
../Sources/Application.c \
../Sources/ESP8266.c \
../Sources/Events.c \
../Sources/main.c \

OBJS += \
./Sources/Application.o \
./Sources/ESP8266.o \
./Sources/Events.o \
./Sources/main.o \

C_DEPS += \
./Sources/Application.d \
./Sources/ESP8266.d \
./Sources/Events.d \
./Sources/main.d \

OBJS_QUOTED += \
"./Sources/Application.o" \
"./Sources/ESP8266.o" \
"./Sources/Events.o" \
"./Sources/main.o" \

C_DEPS_QUOTED += \
"./Sources/Application.d" \
"./Sources/ESP8266.d" \
"./Sources/Events.d" \
"./Sources/main.d" \

OBJS_OS_FORMAT += \
./Sources/Application.o \
./Sources/ESP8266.o \
./Sources/Events.o \
./Sources/main.o \


# Each subdirectory must supply rules for building sources it contributes
Sources/Application.o: ../Sources/Application.c
	@echo 'Building file: $<'
	@echo 'Executing target #1 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Application.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Application.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/ESP8266.o: ../Sources/ESP8266.c
	@echo 'Building file: $<'
	@echo 'Executing target #2 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/ESP8266.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/ESP8266.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/Events.o: ../Sources/Events.c
	@echo 'Building file: $<'
	@echo 'Executing target #3 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/Events.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/Events.o"
	@echo 'Finished building: $<'
	@echo ' '

Sources/main.o: ../Sources/main.c
	@echo 'Building file: $<'
	@echo 'Executing target #4 $<'
	@echo 'Invoking: ARM Ltd Windows GCC C Compiler'
	"$(ARMSourceryDirEnv)/arm-none-eabi-gcc" "$<" @"Sources/main.args" -MMD -MP -MF"$(@:%.o=%.d)" -o"Sources/main.o"
	@echo 'Finished building: $<'
	@echo ' '


