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

using namespace std;


extern jclass jsRuntimeClass;
extern jclass booleanClass;
extern jclass integerClass;
extern jclass longClass;
extern jclass doubleClass;
extern jclass stringClass;
extern jclass mapClass;
extern jclass objectClass;
extern jclass jsFunctionClass;
extern jclass jsObjectClass;


extern jmethodID methodCall;
extern jmethodID isProxy;
extern jmethodID propGetter;
extern jmethodID propSetter;
extern jmethodID getJavaPtr;

extern jmethodID booleanValueOf;
extern jmethodID integerValueOf;
extern jmethodID longValueOf;
extern jmethodID doubleValueOf;
extern jmethodID mapInit;
extern jmethodID mapPut;
extern jmethodID getProxyObj;

extern jmethodID booleanGetValue;
extern jmethodID integerGetValue;
extern jmethodID longGetValue;
extern jmethodID doubleGetValue;


jstring toJavaString(JNIEnv *env, JSContext *context, JSValue value);

jobject toJavaObject(JNIEnv *env, jobject thiz, JSContext *context, JSValueConst value);

JSValue toJSValue(JNIEnv *env, jobject thiz, JSContext *context, jobject value);

#endif //GGJS_UTILS_H
