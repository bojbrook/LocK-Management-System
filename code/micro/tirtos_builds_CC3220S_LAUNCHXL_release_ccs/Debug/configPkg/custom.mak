## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4 linker.cmd package/cfg/release_pem4.oem4

# To simplify configuro usage in makefiles:
#     o create a generic linker command file name 
#     o set modification times of compiler.opt* files to be greater than
#       or equal to the generated config header
#
linker.cmd: package/cfg/release_pem4.xdl
	$(SED) 's"^\"\(package/cfg/release_pem4cfg.cmd\)\"$""\"C:/Users/Sam/Documents/UCSC/2018_Winter/CMPE123A/sazwu/code/micro/tirtos_builds_CC3220S_LAUNCHXL_release_ccs/Debug/configPkg/\1\""' package/cfg/release_pem4.xdl > $@
	-$(SETDATE) -r:max package/cfg/release_pem4.h compiler.opt compiler.opt.defs
