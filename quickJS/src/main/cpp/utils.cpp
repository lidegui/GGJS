//
// Created by Administrator on 2025/10/3.
//

#include "utils.h"

jobject toJavaObject(JNIEnv *env, jobject thiz, JSContext *ctx, JSValueConst value) {
    jobject result;

    switch (JS_VALUE_GET_NORM_TAG(value)) {
        case JS_TAG_EXCEPTION: {
            result = nullptr;
            break;
        }

        case JS_TAG_STRING: {
            result = toJavaString(env, ctx, value);
            break;
        }

        case JS_TAG_BOOL: {
            jvalue v;
            v.z = static_cast<jboolean>(JS_VALUE_GET_BOOL(value));
            result = env->CallStaticObjectMethodA(booleanClass, booleanValueOf, &v);
            break;
        }
        case JS_TAG_INT: {
            jvalue v;
            v.i = static_cast<jint>(JS_VALUE_GET_INT(value));
            result = env->CallStaticObjectMethodA(integerClass, integerValueOf, &v);
            break;
        }
    }

    return result;
}

jstring toJavaString(JNIEnv *env, JSContext *ctx, JSValue value) {
    const char *string = JS_ToCString(ctx, value);
    jstring pJstring = env->NewStringUTF(string);
    JS_FreeCString(ctx, string);
    return pJstring;
}


JSValue toJSValue(JNIEnv *env, jobject thiz, JSContext *ctx, jobject value) {
    if (value == nullptr) {
        return JS_NULL;
    }

    JSValue result;

    if (env->IsInstanceOf(value, stringClass)) {
        const auto s = env->GetStringUTFChars((jstring) value, JNI_FALSE);
        result = JS_NewString(ctx, s);
        env->ReleaseStringUTFChars((jstring) value, s);
    } else {
        result = JS_UNDEFINED;
    }

    return result;
}
