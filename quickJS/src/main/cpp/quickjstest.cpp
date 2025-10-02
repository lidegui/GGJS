//
// Created by Administrator on 2025/10/2.
//

#include <jni.h>
#include <string>
#include <android/log.h>
#include "quickjs/quickjs.h"

#define LOG_TAG "QuickJSTest"
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)

extern "C" JNIEXPORT jstring JNICALL

Java_com_example_quickjs_NativeLib_testQuickJS(JNIEnv *env, jobject /* this */) {
    JSRuntime *rt = JS_NewRuntime();
    if (rt == nullptr) {
        return env->NewStringUTF("Error: Failed to create JS Runtime");
    }

    JSContext *ctx = JS_NewContext(rt);
    if (ctx == nullptr) {
        JS_FreeRuntime(rt);
        return env->NewStringUTF("Error: Failed to create JS Context");
    }


    LOGI("QuickJS hello");
    const char *js_code = "1 + 2 + 3";
    JSValue jsEval = JS_Eval(ctx, js_code, strlen(js_code), "<js_code>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(jsEval)) {
        return env->NewStringUTF("Error: JS Exception");
    }
    const char *result = JS_ToCString(ctx, jsEval);

    if (result == nullptr) {
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
        return env->NewStringUTF("Error: Failed to convert JS Value to C String");
    }

    LOGI("JS Result: %s", result);

    // 清理资源
    JS_FreeCString(ctx, result);
    JS_FreeValue(ctx, jsEval);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);

    return env->NewStringUTF(("Eval result is " + std::string(result)).c_str());
}