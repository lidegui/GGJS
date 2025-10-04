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
jclass longClass;
jclass doubleClass;
jclass mapClass;
jclass objectClass;
jclass jsFunctionClass;
jclass jsObjectClass;

jmethodID methodCall;
jmethodID isProxy;
jmethodID propGetter;
jmethodID propSetter;
jmethodID getJavaPtr;

jmethodID booleanValueOf;
jmethodID integerValueOf;
jmethodID longValueOf;
jmethodID doubleValueOf;
jmethodID mapInit;
jmethodID mapPut;
jmethodID getProxyObj;

jmethodID booleanGetValue;
jmethodID integerGetValue;
jmethodID longGetValue;
jmethodID doubleGetValue;


jint JNI_OnLoad(JavaVM *vm, void *reserved) {
    g_vm = vm;
    JNIEnv *env = nullptr;
    if (vm->GetEnv(reinterpret_cast<void **>(&env), JNI_VERSION_1_6) != JNI_OK) {
        return JNI_ERR;
    }

    jsRuntimeClass = (jclass) env->NewGlobalRef(env->FindClass("com/example/quickjs/JSRuntime"));
    booleanClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Boolean"));
    integerClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Integer"));
    longClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Long"));
    doubleClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Double"));
    stringClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/String"));
    mapClass = (jclass) env->NewGlobalRef(env->FindClass("java/util/HashMap"));
    objectClass = (jclass) env->NewGlobalRef(env->FindClass("java/lang/Object"));
    jsFunctionClass = (jclass) env->NewGlobalRef(env->FindClass("com/example/quickjs/JSFunction"));
    jsObjectClass = (jclass) env->NewGlobalRef(env->FindClass("com/example/quickjs/JSObjectProxy"));

    booleanValueOf = env->GetStaticMethodID(booleanClass, "valueOf", "(Z)Ljava/lang/Boolean;");
    integerValueOf = env->GetStaticMethodID(integerClass, "valueOf", "(I)Ljava/lang/Integer;");
    longValueOf = env->GetStaticMethodID(longClass, "valueOf", "(J)Ljava/lang/Long;");
    doubleValueOf = env->GetStaticMethodID(doubleClass, "valueOf", "(D)Ljava/lang/Double;");
    mapInit = env->GetMethodID(mapClass, "<init>", "()V");
    mapPut = env->GetMethodID(mapClass, "put",
                              "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");

    booleanGetValue = env->GetMethodID(booleanClass, "booleanValue", "()Z");
    integerGetValue = env->GetMethodID(integerClass, "intValue", "()I");
    longGetValue = env->GetMethodID(longClass, "longValue", "()J");
    doubleGetValue = env->GetMethodID(doubleClass, "doubleValue", "()D");


    methodCall = env->GetMethodID(jsRuntimeClass, "methodCall",
                                  "(J[Ljava/lang/Object;)Ljava/lang/Object;");
    isProxy = env->GetMethodID(jsRuntimeClass, "isJSProxyObject", "(J)Z");
    getProxyObj = env->GetMethodID(jsRuntimeClass, "getProxyObject", "(J)Ljava/lang/Object;");
    propGetter = env->GetMethodID(jsRuntimeClass, "propGetter",
                                  "(JLjava/lang/String;)Ljava/lang/Object;");
    propSetter = env->GetMethodID(jsRuntimeClass, "propSetter",
                                  "(JLjava/lang/String;Ljava/lang/Object;)V");
    getJavaPtr = env->GetMethodID(jsObjectClass, "getPtr", "()J");

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
                          LOGE("%s", str);
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

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_quickjs_JSRuntime_nativeCreateJSObj(JNIEnv *env, jobject thiz, jlong ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(ptr);
    JSValue jsValue = JSIObject::wrap(runtime, new JSIObject());
    return reinterpret_cast<jlong>(JS_VALUE_GET_PTR(jsValue));
}

extern "C"
JNIEXPORT jlong JNICALL
Java_com_example_quickjs_JSRuntime_nativeCreateJSFunction(JNIEnv *env, jobject thiz, jlong ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(ptr);
    JSValue jsValue = createJSIFunction(runtime->getCtx(),
                                        [env, runtime](JSContext *ctx, JSValueConst func_obj,
                                                       JSValue this_val, int argc,
                                                       JSValueConst *argv,
                                                       int flags) {
                                            jobjectArray args = env->NewObjectArray(argc,
                                                                                    objectClass,
                                                                                    nullptr);
                                            for (int i = 0; i < argc; i++) {
                                                env->SetObjectArrayElement(args, i,
                                                                           toJavaObject(env,
                                                                                        runtime->javaRuntime,
                                                                                        runtime->getCtx(),
                                                                                        argv[i]));
                                            }
                                            jobject pJobject = env->CallObjectMethod(
                                                    runtime->javaRuntime, methodCall,
                                                    reinterpret_cast<jlong>(JS_VALUE_GET_PTR(
                                                            func_obj)), args);
                                            return toJSValue(env, runtime->javaRuntime,
                                                             runtime->getCtx(), pJobject);
                                        });
    return reinterpret_cast<jlong>(JS_VALUE_GET_PTR(jsValue));
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_JSRuntime_nativeSetGlobalFun(JNIEnv *env, jobject thiz, jlong ptr,
                                                      jstring function_name, jlong callback_ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(ptr);
    const char *functionNameStr = env->GetStringUTFChars(function_name, NULL);
    JSValue globalObj = JS_GetGlobalObject(runtime->getCtx());
    JS_SetPropertyStr(runtime->getCtx(), globalObj, functionNameStr,
                      JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(callback_ptr)));
    env->ReleaseStringUTFChars(function_name, functionNameStr);
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_JSRuntime_nativeSetGlobalObj(JNIEnv *env, jobject thiz, jlong ptr,
                                                      jstring object_name, jlong object_ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(ptr);
    const char *objectNameStr = env->GetStringUTFChars(object_name, NULL);
    JSValue globalObj = JS_GetGlobalObject(runtime->getCtx());
    JS_SetPropertyStr(runtime->getCtx(), globalObj, objectNameStr,
                      JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(object_ptr)));
    JS_FreeValue(runtime->getCtx(), globalObj);
    env->ReleaseStringUTFChars(object_name, objectNameStr);
}


extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_JSRuntime_nativeDestroy(JNIEnv *env, jobject thiz, jlong ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(ptr);
    env->DeleteGlobalRef(runtime->javaRuntime);
    runtime->destroy();
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_JSRuntime_addRefCount(JNIEnv *env, jobject thiz, jlong runtime_ptr,
                                               jlong obj_ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(runtime_ptr);
    JSValue jsValue = JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(obj_ptr));
    JS_DupValue(runtime->getCtx(), jsValue);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_JSRuntime_releaseRefCount(JNIEnv *env, jobject thiz, jlong runtime_ptr,
                                                   jlong obj_ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(runtime_ptr);
    JSValue jsValue = JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(obj_ptr));
    JS_FreeValue(runtime->getCtx(), jsValue);
}
extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_JSRuntime_nativeReleaseObj(JNIEnv *env, jobject thiz, jlong runtime_ptr,
                                                    jlong obj_ptr) {
    GGRuntime *runtime = reinterpret_cast<GGRuntime *>(runtime_ptr);
    JSValue jsValue = JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(obj_ptr));
    JSIObject *jsiObject = (JSIObject *) JS_GetOpaque(jsValue, jsi_class_id);
    if (jsiObject) {
        jsiObject->onDestroy();
    }
}