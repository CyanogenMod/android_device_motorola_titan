BUILD_DATE=$(shell date +'%Y-%m-%d %H:%M:%S')
BUILD_NUM=$(shell cat $(BUILD_NUMBER_FILE))
VERSION_PATH=$(REAL_LOCAL_PATH)/../version

$(LOCAL_PATH)/../version/version.c:$(LOCAL_PATH)/../version/version.h

$(LOCAL_PATH)/../version/version.h:
	@echo ==============================
	@echo "    daemon version : $(DAEMON_VERSION)"
	@echo "    hal version    : $(HAL_VERSION)"
	@echo "    build date     : $(BUILD_DATE)"
	@echo "    use case       : $(usecase)"
	@echo ==============================
	@mkdir -p $(VERSION_PATH)
	@echo "/*" > $(VERSION_PATH)/version.c
	@echo " * This file is automatic generated," >> $(VERSION_PATH)/version.c
	@echo " * please do not make modification" >> $(VERSION_PATH)/version.c
	@echo " */" >> $(VERSION_PATH)/version.c
	@echo "" >> $(VERSION_PATH)/version.c
	@echo "static const char daemon_version_str[] = \"$(DAEMON_VERSION)\";" >> $(VERSION_PATH)/version.c
	@echo "static const char hal_version_str[] = \"$(HAL_VERSION)\";" >> $(VERSION_PATH)/version.c
	@echo "static const char date_str[] = \"$(BUILD_DATE)\";" >> $(VERSION_PATH)/version.c
	@echo "const char* get_daemon_version(void)" >> $(VERSION_PATH)/version.c
	@echo "{" >> $(VERSION_PATH)/version.c
	@echo "	return daemon_version_str;" >> $(VERSION_PATH)/version.c
	@echo "}" >> $(VERSION_PATH)/version.c
	@echo "const char* get_hal_version(void)" >> $(VERSION_PATH)/version.c
	@echo "{" >> $(VERSION_PATH)/version.c
	@echo "	return hal_version_str;" >> $(VERSION_PATH)/version.c
	@echo "}" >> $(VERSION_PATH)/version.c
	@echo "const char* get_build_date(void)" >> $(VERSION_PATH)/version.c
	@echo "{" >> $(VERSION_PATH)/version.c
	@echo "	return date_str;" >> $(VERSION_PATH)/version.c
	@echo "}" >> $(VERSION_PATH)/version.c
	@echo ""  >> $(VERSION_PATH)/version.c
	@echo "/*" > $(VERSION_PATH)/version.h
	@echo " * This file is automatic generated," >> $(VERSION_PATH)/version.h
	@echo " * please do not make modification" >> $(VERSION_PATH)/version.h
	@echo " */" >> $(VERSION_PATH)/version.h
	@echo "" >> $(VERSION_PATH)/version.h
	@echo "#ifdef __cplusplus" >> $(VERSION_PATH)/version.h
	@echo "extern \"C\" {"  >> $(VERSION_PATH)/version.h
	@echo "#endif" >> $(VERSION_PATH)/version.h
	@echo "const char* get_daemon_version(void);" >> $(VERSION_PATH)/version.h
	@echo "const char* get_hal_version(void);" >> $(VERSION_PATH)/version.h
	@echo "const char* get_build_date(void);" >> $(VERSION_PATH)/version.h
	@echo "#ifdef __cplusplus" >> $(VERSION_PATH)/version.h
	@echo "};"  >> $(VERSION_PATH)/version.h
	@echo "#endif" >> $(VERSION_PATH)/version.h

