################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/chiong/work/c/lcclib/unit_test/test_main.c 

OBJS += \
./unit_test/test_main.o 

C_DEPS += \
./unit_test/test_main.d 


# Each subdirectory must supply rules for building sources it contributes
unit_test/test_main.o: /Users/chiong/work/c/lcclib/unit_test/test_main.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/test-lcc-pc/../../inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


