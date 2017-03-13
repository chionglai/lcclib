################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/chiong/work/c/lcclib/src/util/bitmap.c \
/Users/chiong/work/c/lcclib/src/util/buffer.c \
/Users/chiong/work/c/lcclib/src/util/file.c \
/Users/chiong/work/c/lcclib/src/util/stack.c \
/Users/chiong/work/c/lcclib/src/util/std.c 

OBJS += \
./src/util/bitmap.o \
./src/util/buffer.o \
./src/util/file.o \
./src/util/stack.o \
./src/util/std.o 

C_DEPS += \
./src/util/bitmap.d \
./src/util/buffer.d \
./src/util/file.d \
./src/util/stack.d \
./src/util/std.d 


# Each subdirectory must supply rules for building sources it contributes
src/util/bitmap.o: /Users/chiong/work/c/lcclib/src/util/bitmap.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/util/buffer.o: /Users/chiong/work/c/lcclib/src/util/buffer.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/util/file.o: /Users/chiong/work/c/lcclib/src/util/file.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/util/stack.o: /Users/chiong/work/c/lcclib/src/util/stack.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/util/std.o: /Users/chiong/work/c/lcclib/src/util/std.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


