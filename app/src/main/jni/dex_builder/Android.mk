LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
LOCAL_MODULE            := dex_builder
LOCAL_CPPFLAGS          := -std=c++20
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES         := dex_builder.cc slicer/writer.cc slicer/reader.cc slicer/dex_ir.cc slicer/common.cc \
                           slicer/dex_format.cc slicer/dex_utf8.cc slicer/dex_bytecode.cc
LOCAL_EXPORT_LDLIBS     := -lz
LOCAL_LDLIBS            := -lz
LOCAL_CFLAGS            := -fvisibility=default -fvisibility-inlines-hidden -flto
LOCAL_LDFLAGS           := -flto
include $(BUILD_SHARED_LIBRARY)

include $(CLEAR_VARS)
LOCAL_MODULE            := dex_builder_static
LOCAL_CPPFLAGS          := -std=c++20
LOCAL_C_INCLUDES        := $(LOCAL_PATH)/include
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH)/include
LOCAL_SRC_FILES         := dex_builder.cc slicer/writer.cc slicer/reader.cc slicer/dex_ir.cc slicer/common.cc \
                           slicer/dex_format.cc slicer/dex_utf8.cc slicer/dex_bytecode.cc
LOCAL_EXPORT_LDLIBS     := -lz
include $(BUILD_STATIC_LIBRARY)
