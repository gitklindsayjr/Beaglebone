################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Each subdirectory must supply rules for building sources it contributes
src/%.obj: ../src/%.c $(GEN_OPTS) | $(GEN_FILES) $(GEN_MISC_FILES)
	@echo 'Building file: "$<"'
	@echo 'Invoking: PRU Compiler'
	"/opt/ti/ccs1020/ccs/tools/compiler/ti-cgt-pru_2.3.3/bin/clpru" -v3 --include_path="/opt/ti/ccs1020/ccs/ccs_base/pru/include" --include_path="/home/pops/ti/pru-software-support-package/include" --include_path="/home/pops/MicroControllers/BeaglebonePrus/PruLibrary" --include_path="/opt/ti/ccs1020/ccs/tools/compiler/ti-cgt-pru_2.3.3/include" --define=am3359 --define=pru0 -g --diag_warning=225 --diag_wrap=off --display_error_number --endian=little --hardware_mac=on --preproc_with_compile --preproc_dependency="src/$(basename $(<F)).d_raw" --obj_directory="src" $(GEN_OPTS__FLAG) "$(shell echo $<)"
	@echo 'Finished building: "$<"'
	@echo ' '


