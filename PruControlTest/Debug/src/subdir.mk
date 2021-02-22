################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/pru_control_class.cpp \
../src/test_pru_control.cpp 

OBJS += \
./src/pru_control_class.o \
./src/test_pru_control.o 

CPP_DEPS += \
./src/pru_control_class.d \
./src/test_pru_control.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	arm-linux-gnueabihf-g++ -I"/home/pops/MicroControllers/Beaglebone/BBB_ClassLib/src" -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


