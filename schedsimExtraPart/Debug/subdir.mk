################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../barrier.c \
../main.c \
../sched.c \
../sched_fcfs.c \
../sched_prio.c \
../sched_prio_multi.c \
../sched_rr.c \
../sched_sjf.c \
../slist.c 

OBJS += \
./barrier.o \
./main.o \
./sched.o \
./sched_fcfs.o \
./sched_prio.o \
./sched_prio_multi.o \
./sched_rr.o \
./sched_sjf.o \
./slist.o 

C_DEPS += \
./barrier.d \
./main.d \
./sched.d \
./sched_fcfs.d \
./sched_prio.d \
./sched_prio_multi.d \
./sched_rr.d \
./sched_sjf.d \
./slist.d 


# Each subdirectory must supply rules for building sources it contributes
%.o: ../%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross GCC Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


