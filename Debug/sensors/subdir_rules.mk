################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
sensors/accelerometer.obj: ../sensors/accelerometer.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/TivaWare_C_Series-2.0.1.11577" --include_path="C:/ti/TivaWare_C_Series-2.1.3.156" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="sensors/accelerometer.d" --obj_directory="sensors" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sensors/bmp180_pressure_temperature.obj: ../sensors/bmp180_pressure_temperature.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/TivaWare_C_Series-2.0.1.11577" --include_path="C:/ti/TivaWare_C_Series-2.1.3.156" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="sensors/bmp180_pressure_temperature.d" --obj_directory="sensors" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '

sensors/i2c0.obj: ../sensors/i2c0.c $(GEN_OPTS) $(GEN_HDRS)
	@echo 'Building file: $<'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=FPv4SPD16 -me --include_path="C:/ti/TivaWare_C_Series-2.0.1.11577" --include_path="C:/ti/TivaWare_C_Series-2.1.3.156" --include_path="C:/ti/ccsv6/tools/compiler/ti-cgt-arm_15.12.1.LTS/include" -g --gcc --define=ccs="ccs" --define=PART_TM4C123GH6PM --diag_wrap=off --diag_warning=225 --display_error_number --abi=eabi --preproc_with_compile --preproc_dependency="sensors/i2c0.d" --obj_directory="sensors" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: $<'
	@echo ' '


