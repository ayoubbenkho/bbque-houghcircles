
ifdef CONFIG_SAMPLES_OPENCV_HOUGHCIRCLES

# Targets provided by this project
.PHONY: samples_opencv_houghcircles clean_samples_opencv_houghcircles

# Add this to the "samples" targets
samples_opencv: samples_opencv_houghcircles
clean_samples_opencv: clean_samples_opencv_houghcircles

MODULE_SAMPLES_OPENCV_HOUGHCIRCLES=samples/opencv/HoughCircles

samples_opencv_houghcircles: external
	@echo
	@echo "==== Building HoughCircles ($(BUILD_TYPE)) ===="
	@echo " Using GCC    : $(CC)"
	@echo " Target flags : $(TARGET_FLAGS)"
	@echo " Sysroot      : $(PLATFORM_SYSROOT)"
	@echo " BOSP Options : $(CMAKE_COMMON_OPTIONS)"
	@[ -d $(MODULE_SAMPLES_OPENCV_HOUGHCIRCLES)/build/$(BUILD_TYPE) ] || \
		mkdir -p $(MODULE_SAMPLES_OPENCV_HOUGHCIRCLES)/build/$(BUILD_TYPE) || \
		exit 1
	@cd $(MODULE_SAMPLES_OPENCV_HOUGHCIRCLES)/build/$(BUILD_TYPE) && \
		CC=$(CC) CFLAGS="$(TARGET_FLAGS)" \
		CXX=$(CXX) CXXFLAGS="$(TARGET_FLAGS)" \
		cmake $(CMAKE_COMMON_OPTIONS) ../.. || \
		exit 1
	@cd $(MODULE_SAMPLES_OPENCV_HOUGHCIRCLES)/build/$(BUILD_TYPE) && \
		make -j$(CPUS) install || \
		exit 1

clean_samples_opencv_houghcircles:
	@echo
	@echo "==== Clean-up HoughCircles Application ===="
	@[ ! -f $(BUILD_DIR)/usr/bin/houghcircles ] || \
		rm -f $(BUILD_DIR)/etc/bbque/recipes/HoughCircles*; \
		rm -f $(BUILD_DIR)/usr/bin/houghcircles*
	@rm -rf $(MODULE_SAMPLES_OPENCV_HOUGHCIRCLES)/build
	@echo

else # CONFIG_SAMPLES_OPENCV_HOUGHCIRCLES

samples_opencv_houghcircles:
	$(warning HoughCircles module disabled by BOSP configuration)
	$(error BOSP compilation failed)

endif # CONFIG_SAMPLES_OPENCV_HOUGHCIRCLES

