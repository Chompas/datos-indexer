################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../src/Coder.cpp \
../src/Parser.cpp \
../src/TerminoRegister.cpp \
../src/TpDatos.cpp 

OBJS += \
./src/Coder.o \
./src/Parser.o \
./src/TerminoRegister.o \
./src/TpDatos.o 

CPP_DEPS += \
./src/Coder.d \
./src/Parser.d \
./src/TerminoRegister.d \
./src/TpDatos.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C++ Compiler'
	g++ -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


