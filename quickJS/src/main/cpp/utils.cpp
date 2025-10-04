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
        case JS_TAG_FLOAT64: {
            jvalue v;
            double d = JS_VALUE_GET_FLOAT64(value);
            bool isInteger = floor(d) == d;
            if (isInteger) {
                v.j = static_cast<jlong>(d);
                result = env->CallStaticObjectMethodA(longClass, longValueOf, &v);
            } else {
                v.d = static_cast<jdouble>(d);
                result = env->CallStaticObjectMethodA(doubleClass, doubleValueOf, &v);
            }
            break;
        }

        case JS_TAG_OBJECT: {
            jlong value_ptr = reinterpret_cast<jlong>(JS_VALUE_GET_PTR(value));
            jboolean isProxyObj = env->CallBooleanMethod(thiz, isProxy,
                                                         reinterpret_cast<jlong>(value_ptr));
            if (isProxyObj) {
                result = env->CallObjectMethod(thiz, getProxyObj, value_ptr);
            } else {
                jobject mapObj = env->NewObject(mapClass, mapInit);
                JSPropertyEnum *props;
                uint32_t len;
                JS_GetOwnPropertyNames(ctx, &props, &len, value,
                                       JS_GPN_STRING_MASK | JS_GPN_ENUM_ONLY);
                for (uint32_t i = 0; i < len; i++) {
                    JSAtom atom = props[i].atom;
                    const char *key = JS_AtomToCString(ctx, atom);
                    JSValue propVal = JS_GetProperty(ctx, value, atom);
                    env->CallVoidMethod(mapObj, mapPut, env->NewStringUTF(key),
                                        toJavaObject(env, thiz, ctx, propVal));
                    JS_FreeAtom(ctx, atom);
                    JS_FreeCString(ctx, key);
                    result = env->CallObjectMethod(thiz, getJavaPtr, value_ptr);
                }
                js_free(ctx, props);
            }
            break;
        }
        default:
            result = nullptr;
            break;
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
    } else if (env->IsInstanceOf(value, booleanClass)) {
        result = JS_NewBool(ctx, env->CallBooleanMethod(value, booleanGetValue));
    } else if (env->IsInstanceOf(value, integerClass)) {
        result = JS_NewInt32(ctx, env->CallIntMethod(value, integerGetValue));
    } else if (env->IsInstanceOf(value, longClass)) {
        result = JS_NewInt64(ctx, env->CallLongMethod(value, longGetValue));
    } else if (env->IsInstanceOf(value, doubleClass)) {
        result = JS_NewFloat64(ctx, env->CallDoubleMethod(value, doubleGetValue));
    } else if (env->IsInstanceOf(value, jsObjectClass)) {
        result = JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(
                env->CallLongMethod(value, getJavaPtr)));
    } else {
        result = JS_UNDEFINED;
    }

    return result;
}
