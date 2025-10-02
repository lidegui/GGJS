//
// Created by Administrator on 2025/10/3.
//

#ifndef GGJS_UTILS_H
#define GGJS_UTILS_H

#include <android/log.h>
#include "string"
#include "jni.h"
#include "quickjs/quickjs.h"

#define LOG_TAG "GGJS-Clog"
#define LOGE(fmt, ...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOGI(fmt, ...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##__VA_ARGS__)
#define LOGD(fmt, ...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##__VA_ARGS__)

class utils {

};


#endif //GGJS_UTILS_H
