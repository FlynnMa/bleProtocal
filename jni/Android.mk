# Copyright (C) 2014 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE    := native-ev-jni
LOCAL_SRC_FILES := protocal/bicProcess.c protocal/protocal.c protocal/protocalApi.c \
					list/list.c trace/trace.c bicJni.c

LOCAL_C_INCLUDES = $(LOCAL_PATH) $(LOCAL_PATH)/protocal $(LOCAL_PATH)/list $(LOCAL_PATH)/trace

# for function libs
LOCAL_LDLIBS    +=
# for logging libs
LOCAL_LDLIBS    += -llog
# for native windows
LOCAL_LDLIBS    += -landroid

LOCAL_CFLAGS    += -UNDEBUG -D__WITH_LOG_FUNCTION__ -D__LOG_TO_LOGCAT__

include $(BUILD_SHARED_LIBRARY)
