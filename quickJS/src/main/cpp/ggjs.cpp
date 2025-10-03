//
// Created by Administrator on 2025/10/3.
//

#include <jni.h>
#include <string>
#include "jsi.h"
#include "android/log.h"
#include "utils.h"

JavaVM *g_vm = nullptr;

jclass jsRuntimeClass;
jclass booleanClass;
jclass integerClass;
jclass stringClass;

jmethodID propGetter;
jmethodID propSetter;
jmethodID getFunPtr;

jmethodID booleanValueOf;
jmethodID integerValueOf;

jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vm = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jsRuntimeClass = (jclass) env->NewGlobalRef(env->FindClass("com/example/quickjs/JSRuntime"));
    booleanClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Boolean"));
    integerClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Integer"));
    stringClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/String"));

    booleanValueOf = env->GetStaticMethodID(booleanClass, "valueOf", "(Z)Ljava/lang/Boolean;");
    integerValueOf = env->GetStaticMethodID(integerClass, "valueOf", "(I)Ljava/lang/Integer;");

    propGetter = env->GetMethodID(jsRuntimeClass, "propGetter",
                                  "(JLjava/lang/String;)Ljava/lang/Object;");
    propSetter = env->GetMethodID(jsRuntimeClass, "propSetter",
                                  "(JLjava/lang/String;Ljava/lang/Object;)V");

    return JNI_VERSION_1_6;
}

JNIEnv *GetJniEnv() {
    if (g_vm == nullptr) return nullptr;
    JNIEnv *env = nullptr;
    jint res = g_vm->GetEnv((void **) &env, JNI_VERSION_1_6);
    if (res == JNI_OK) {
        return env;
    } else if (res == JNI_EDETACHED) {
        if (g_vm->AttachCurrentThread(&env, nullptr) == 0) {
            return env;
        }
    }

    return nullptr;
}

void GGRuntime::registerGlobalFun() {
    JSValue global = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, global, "print",
                      createJSIFunction(ctx, [this](JSContext *ctx, JSValueConst func_obj,
                                                    JSValue this_val, int argc, JSValueConst *argv,
                                                    int flags) {
                          const char *str = JS_ToCString(ctx, argv[0]);
                          LOGE("print %s", str);
                          JS_FreeCString(ctx, str);
                          return JSIObject::wrap(this, new JSIObject());
                      }));
    JS_FreeValue(ctx, global);
}

void JSIObject::set(std::string propNme, JSValue value) {
    JNIEnv *env = GetJniEnv();
    jstring pName = env->NewStringUTF(propNme.c_str());
    env->CallVoidMethod(rt->javaRuntime, propSetter,
                        reinterpret_cast<jlong>(JS_VALUE_GET_PTR(js_value)), pName,
                        toJavaObject(env, rt->javaRuntime, rt->getCtx(), value));
}

JSValue JSIObject::get(std::string propName) {
    JNIEnv *env = GetJniEnv();
    auto it = functionCache.find(propName);
    if (it != functionCache.end()) {
        JSValue jsValue = JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(it->second));
        if (JS_IsFunction(ctx, jsValue)) {
            return jsValue;
        }
    }
    jlong ptr = reinterpret_cast<jlong>(JS_VALUE_GET_PTR(js_value));
    jstring pName = env->NewStringUTF(propName.c_str());
    jobject result = env->CallObjectMethod(rt->javaRuntime, propGetter, ptr, pName);
    JSValue val = toJSValue(env, rt->javaRuntime, rt->getCtx(), result);
    if (JS_IsFunction(ctx, val)) {
        JS_DupValue(ctx, val);
        LOGE("save function %s", propName.c_str());
        functionCache[propName] = reinterpret_cast<jlong>(JS_VALUE_GET_PTR(val));
        rt->objFunctionCache.insert(reinterpret_cast<jlong>(JS_VALUE_GET_PTR(val)));
    }
    return val;
}


extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_quickjs_JSRuntime_nativeInitRuntime(JNIEnv *env, jobject thiz) {
    GGRuntime *pGgRuntime = new GGRuntime();
    pGgRuntime->javaRuntime = env->NewGlobalRef(thiz);
    pGgRuntime->init();
    return reinterpret_cast<jlong>(pGgRuntime);
}



extern "C"
JNIEXPORT jobject JNICALL
Java_com_example_quickjs_JSRuntime_nativeEvalCode(JNIEnv *env, jobject thiz, jlong ptr,
                                                  jstring code, jstring file_name) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(ptr);
    const char *codeStr = env->GetStringUTFChars(code, NULL);
    const char *fileNameStr = env->GetStringUTFChars(file_name, NULL);
    JSValue value = runtime->eval(codeStr, fileNameStr);
    env->ReleaseStringUTFChars(code, codeStr);
    env->ReleaseStringUTFChars(file_name, fileNameStr);
    jobject result = toJavaObject(env, runtime->javaRuntime, runtime->getCtx(), value);
    return result;
}