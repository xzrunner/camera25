INNER_SAVED_LOCAL_PATH := $(LOCAL_PATH)

LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := camera25

LOCAL_C_INCLUDES  := \
	${LUA_SRC_PATH} \
	${SM_SRC_PATH} \

LOCAL_SRC_FILES := \
	$(subst $(LOCAL_PATH)/,,$(shell find $(LOCAL_PATH) -name "*.c" -print)) \

LOCAL_STATIC_LIBRARIES := \
	sm \

include $(BUILD_STATIC_LIBRARY)	

LOCAL_PATH := $(INNER_SAVED_LOCAL_PATH)