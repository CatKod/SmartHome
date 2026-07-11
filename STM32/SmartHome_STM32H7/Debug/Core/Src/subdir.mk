################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/adc.c \
../Core/Src/buzzer.c \
../Core/Src/dht11.c \
../Core/Src/gpio.c \
../Core/Src/keypad.c \
../Core/Src/light_sensor.c \
../Core/Src/main.c \
../Core/Src/pir_sensor.c \
../Core/Src/raindrop_sensor.c \
../Core/Src/relay.c \
../Core/Src/rfid_rc522.c \
../Core/Src/servo_sg90.c \
../Core/Src/sn74hc595.c \
../Core/Src/sound_sensor.c \
../Core/Src/spi.c \
../Core/Src/stepper_28byj.c \
../Core/Src/stm32h7xx_hal_msp.c \
../Core/Src/stm32h7xx_it.c \
../Core/Src/syscalls.c \
../Core/Src/sysmem.c \
../Core/Src/system_stm32h7xx.c \
../Core/Src/thermistor.c \
../Core/Src/tim.c \
../Core/Src/usart.c 

OBJS += \
./Core/Src/adc.o \
./Core/Src/buzzer.o \
./Core/Src/dht11.o \
./Core/Src/gpio.o \
./Core/Src/keypad.o \
./Core/Src/light_sensor.o \
./Core/Src/main.o \
./Core/Src/pir_sensor.o \
./Core/Src/raindrop_sensor.o \
./Core/Src/relay.o \
./Core/Src/rfid_rc522.o \
./Core/Src/servo_sg90.o \
./Core/Src/sn74hc595.o \
./Core/Src/sound_sensor.o \
./Core/Src/spi.o \
./Core/Src/stepper_28byj.o \
./Core/Src/stm32h7xx_hal_msp.o \
./Core/Src/stm32h7xx_it.o \
./Core/Src/syscalls.o \
./Core/Src/sysmem.o \
./Core/Src/system_stm32h7xx.o \
./Core/Src/thermistor.o \
./Core/Src/tim.o \
./Core/Src/usart.o 

C_DEPS += \
./Core/Src/adc.d \
./Core/Src/buzzer.d \
./Core/Src/dht11.d \
./Core/Src/gpio.d \
./Core/Src/keypad.d \
./Core/Src/light_sensor.d \
./Core/Src/main.d \
./Core/Src/pir_sensor.d \
./Core/Src/raindrop_sensor.d \
./Core/Src/relay.d \
./Core/Src/rfid_rc522.d \
./Core/Src/servo_sg90.d \
./Core/Src/sn74hc595.d \
./Core/Src/sound_sensor.d \
./Core/Src/spi.d \
./Core/Src/stepper_28byj.d \
./Core/Src/stm32h7xx_hal_msp.d \
./Core/Src/stm32h7xx_it.d \
./Core/Src/syscalls.d \
./Core/Src/sysmem.d \
./Core/Src/system_stm32h7xx.d \
./Core/Src/thermistor.d \
./Core/Src/tim.d \
./Core/Src/usart.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/%.o Core/Src/%.su Core/Src/%.cyclo: ../Core/Src/%.c Core/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src

clean-Core-2f-Src:
	-$(RM) ./Core/Src/adc.cyclo ./Core/Src/adc.d ./Core/Src/adc.o ./Core/Src/adc.su ./Core/Src/buzzer.cyclo ./Core/Src/buzzer.d ./Core/Src/buzzer.o ./Core/Src/buzzer.su ./Core/Src/dht11.cyclo ./Core/Src/dht11.d ./Core/Src/dht11.o ./Core/Src/dht11.su ./Core/Src/gpio.cyclo ./Core/Src/gpio.d ./Core/Src/gpio.o ./Core/Src/gpio.su ./Core/Src/keypad.cyclo ./Core/Src/keypad.d ./Core/Src/keypad.o ./Core/Src/keypad.su ./Core/Src/light_sensor.cyclo ./Core/Src/light_sensor.d ./Core/Src/light_sensor.o ./Core/Src/light_sensor.su ./Core/Src/main.cyclo ./Core/Src/main.d ./Core/Src/main.o ./Core/Src/main.su ./Core/Src/pir_sensor.cyclo ./Core/Src/pir_sensor.d ./Core/Src/pir_sensor.o ./Core/Src/pir_sensor.su ./Core/Src/raindrop_sensor.cyclo ./Core/Src/raindrop_sensor.d ./Core/Src/raindrop_sensor.o ./Core/Src/raindrop_sensor.su ./Core/Src/relay.cyclo ./Core/Src/relay.d ./Core/Src/relay.o ./Core/Src/relay.su ./Core/Src/rfid_rc522.cyclo ./Core/Src/rfid_rc522.d ./Core/Src/rfid_rc522.o ./Core/Src/rfid_rc522.su ./Core/Src/servo_sg90.cyclo ./Core/Src/servo_sg90.d ./Core/Src/servo_sg90.o ./Core/Src/servo_sg90.su ./Core/Src/sn74hc595.cyclo ./Core/Src/sn74hc595.d ./Core/Src/sn74hc595.o ./Core/Src/sn74hc595.su ./Core/Src/sound_sensor.cyclo ./Core/Src/sound_sensor.d ./Core/Src/sound_sensor.o ./Core/Src/sound_sensor.su ./Core/Src/spi.cyclo ./Core/Src/spi.d ./Core/Src/spi.o ./Core/Src/spi.su ./Core/Src/stepper_28byj.cyclo ./Core/Src/stepper_28byj.d ./Core/Src/stepper_28byj.o ./Core/Src/stepper_28byj.su ./Core/Src/stm32h7xx_hal_msp.cyclo ./Core/Src/stm32h7xx_hal_msp.d ./Core/Src/stm32h7xx_hal_msp.o ./Core/Src/stm32h7xx_hal_msp.su ./Core/Src/stm32h7xx_it.cyclo ./Core/Src/stm32h7xx_it.d ./Core/Src/stm32h7xx_it.o ./Core/Src/stm32h7xx_it.su ./Core/Src/syscalls.cyclo ./Core/Src/syscalls.d ./Core/Src/syscalls.o ./Core/Src/syscalls.su ./Core/Src/sysmem.cyclo ./Core/Src/sysmem.d ./Core/Src/sysmem.o ./Core/Src/sysmem.su ./Core/Src/system_stm32h7xx.cyclo ./Core/Src/system_stm32h7xx.d ./Core/Src/system_stm32h7xx.o ./Core/Src/system_stm32h7xx.su ./Core/Src/thermistor.cyclo ./Core/Src/thermistor.d ./Core/Src/thermistor.o ./Core/Src/thermistor.su ./Core/Src/tim.cyclo ./Core/Src/tim.d ./Core/Src/tim.o ./Core/Src/tim.su ./Core/Src/usart.cyclo ./Core/Src/usart.d ./Core/Src/usart.o ./Core/Src/usart.su

.PHONY: clean-Core-2f-Src

