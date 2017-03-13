################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/chiong/work/c/lcclib/src/dsp/apsigm.c \
/Users/chiong/work/c/lcclib/src/dsp/chirp.c 

OBJS += \
./src/dsp/apsigm.o \
./src/dsp/chirp.o 

C_DEPS += \
./src/dsp/apsigm.d \
./src/dsp/chirp.d 


# Each subdirectory must supply rules for building sources it contributes
src/dsp/apsigm.o: /Users/chiong/work/c/lcclib/src/dsp/apsigm.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '

src/dsp/chirp.o: /Users/chiong/work/c/lcclib/src/dsp/chirp.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../inc" -I"/Users/chiong/work/c/lcclib/proj/lcc-osx/../../.." -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


