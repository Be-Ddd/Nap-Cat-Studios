LOCAL_PATH  := $(call my-dir)
CURR_DEPTH  := ..
CUGL_OFFSET := ../../../../../../cugl

########################
#
# Main Application Entry
#
########################
CUGL_PATH := $(LOCAL_PATH)/$(CURR_DEPTH)/$(CUGL_OFFSET)
SDL3_PATH := $(CUGL_PATH)/sdlapp
PROJ_PATH := $(LOCAL_PATH)/${CURR_DEPTH}/../../../../..

include $(CLEAR_VARS)

LOCAL_MODULE := main
LOCAL_C_INCLUDES := $(CUGL_PATH)/include
LOCAL_C_INCLUDES += $(SDL3_PATH)/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/libdatachannel/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/box2d/include
LOCAL_C_INCLUDES += $(CUGL_PATH)/external/poly2tri
LOCAL_C_INCLUDES += $(PROJ_PATH)/source


# Add your application source files here.
LOCAL_PATH = $(PROJ_PATH)
LOCAL_SRC_FILES := $(subst $(LOCAL_PATH)/,, \
	$(LOCAL_PATH)/source/InputController.cpp \
	$(LOCAL_PATH)/source/CollisionController.cpp \
	$(LOCAL_PATH)/source/Player.cpp \
	$(LOCAL_PATH)/source/ObjectModel.cpp \
	$(LOCAL_PATH)/source/ValuableSet.cpp \
	$(LOCAL_PATH)/source/GameScene.cpp \
	$(LOCAL_PATH)/source/App.cpp)

# Link in CUGL and SDL3
LOCAL_STATIC_LIBRARIES := CUGL

LOCAL_LDLIBS := -lEGL -lGLESv1_CM -lGLESv2 -lGLESv3 -lOpenSLES -llog -landroid
include $(BUILD_SHARED_LIBRARY)
