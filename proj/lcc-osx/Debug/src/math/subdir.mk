################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/chiong/work/c/lcclib/src/math/fimath.c 

OBJS += \
./src/math/fimath.o 

C_DEPS += \
./src/math/fimath.d 


# Each subdirectory must supply rules for building sources it contributes
src/math/fimath.o: /Users/chiong/work/c/lcclib/src/math/fimath.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


