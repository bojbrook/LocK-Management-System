################################################################################
# Automatically-generated file. Do not edit!
################################################################################

SHELL = cmd.exe

# Each subdirectory must supply rules for building sources it contributes
CC3220S_LAUNCHXL.obj: ../CC3220S_LAUNCHXL.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O2 --include_path="C:/Users/Sam/Documents/UCSC/2018_Winter/CMPE123A/sazwu/code/micro/empty_CC3220S_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc32xx_sdk_1_60_00_04/source/ti/posix/ccs" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="CC3220S_LAUNCHXL.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

PN532.obj: ../PN532.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O2 --include_path="C:/Users/Sam/Documents/UCSC/2018_Winter/CMPE123A/sazwu/code/micro/empty_CC3220S_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc32xx_sdk_1_60_00_04/source/ti/posix/ccs" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="PN532.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

empty.obj: ../empty.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O2 --include_path="C:/Users/Sam/Documents/UCSC/2018_Winter/CMPE123A/sazwu/code/micro/empty_CC3220S_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc32xx_sdk_1_60_00_04/source/ti/posix/ccs" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="empty.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

main_tirtos.obj: ../main_tirtos.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O2 --include_path="C:/Users/Sam/Documents/UCSC/2018_Winter/CMPE123A/sazwu/code/micro/empty_CC3220S_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc32xx_sdk_1_60_00_04/source/ti/posix/ccs" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="main_tirtos.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '

uart_term.obj: ../uart_term.c $(GEN_OPTS) | $(GEN_HDRS)
	@echo 'Building file: "$<"'
	@echo 'Invoking: ARM Compiler'
	"C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/bin/armcl" -mv7M4 --code_state=16 --float_support=vfplib -me -O2 --include_path="C:/Users/Sam/Documents/UCSC/2018_Winter/CMPE123A/sazwu/code/micro/empty_CC3220S_LAUNCHXL_tirtos_ccs" --include_path="C:/ti/simplelink_cc32xx_sdk_1_60_00_04/source/ti/posix/ccs" --include_path="C:/ti/ccsv7/tools/compiler/ti-cgt-arm_16.9.6.LTS/include" -g --diag_warning=225 --diag_warning=255 --diag_wrap=off --display_error_number --gen_func_subsections=on --preproc_with_compile --preproc_dependency="uart_term.d_raw" $(GEN_OPTS__FLAG) "$<"
	@echo 'Finished building: "$<"'
	@echo ' '


