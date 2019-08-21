# invoke SourceDir generated makefile for release.pem4
release.pem4: .libraries,release.pem4
.libraries,release.pem4: package/cfg/release_pem4.xdl
	$(MAKE) -f C:\Users\Sam\Documents\UCSC\2018_Winter\CMPE123A\sazwu\code\micro\tirtos_builds_CC3220S_LAUNCHXL_release_ccs/src/makefile.libs

clean::
	$(MAKE) -f C:\Users\Sam\Documents\UCSC\2018_Winter\CMPE123A\sazwu\code\micro\tirtos_builds_CC3220S_LAUNCHXL_release_ccs/src/makefile.libs clean

