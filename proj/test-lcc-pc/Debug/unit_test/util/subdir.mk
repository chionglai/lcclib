################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/chiong/work/c/lcclib/unit_test/util/test_bit.c \
/Users/chiong/work/c/lcclib/unit_test/util/test_buffer.c \
/Users/chiong/work/c/lcclib/unit_test/util/test_stack.c \
/Users/chiong/work/c/lcclib/unit_test/util/test_util.c 

OBJS += \
./unit_test/util/test_bit.o \
./unit_test/util/test_buffer.o \
./unit_test/util/test_stack.o \
./unit_test/util/test_util.o 

C_DEPS += \
./unit_test/util/test_bit.d \
./unit_test/util/test_buffer.d \
./unit_test/util/test_stack.d \
./unit_test/util/test_util.d 


# Each subdirectory must supply rules for building sources it contributes
unit_test/util/test_bit.o: /Users/chiong/work/c/lcclib/unit_test/util/test_bit.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/test-lcc-pc/../../inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

unit_test/util/test_buffer.o: /Users/chiong/work/c/lcclib/unit_test/util/test_buffer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/test-lcc-pc/../../inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

unit_test/util/test_stack.o: /Users/chiong/work/c/lcclib/unit_test/util/test_stack.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/test-lcc-pc/../../inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

unit_test/util/test_util.o: /Users/chiong/work/c/lcclib/unit_test/util/test_util.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/test-lcc-pc/../../inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


