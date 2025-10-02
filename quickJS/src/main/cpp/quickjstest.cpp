//
// Created by Administrator on 2025/10/2.
//

#include <jni.h>
#include <string>
#include <android/log.h>
#include "quickjs/quickjs.h"
#include "quickjs/quickjs-libc.h"
#include "utils.h"

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


static JSValue js_print(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv) {
    std::string res;

    for (int i = 0; i < argc; i++) {
        const char *str = JS_ToCString(ctx, argv[i]);
        if (str == nullptr) {
            continue;
        }
        if (i != 0) {
            res.append(" ");
        }
        res.append(str);
        JS_FreeCString(ctx, str);
    }
    LOGD("%s", res.c_str());
    return JS_UNDEFINED;
}

extern "C" JNIEXPORT void JNICALL
Java_com_example_quickjs_NativeLib_testJsInvokeC(JNIEnv *env, jobject thiz) {
    JSRuntime *rt = JS_NewRuntime();
    if (rt == nullptr) {
        return;
    }

    JSContext *ctx = JS_NewContext(rt);
    if (ctx == nullptr) {
        JS_FreeRuntime(rt);
        return;
    }

    js_std_add_helpers(ctx, 0, NULL);
    JSValue globalObject = JS_GetGlobalObject(ctx);
    JSValue cPrint = JS_NewCFunction(ctx, js_print, "print", 1);
    JSAtom atomPrint = JS_NewAtom(ctx, "print");
    JS_SetProperty(ctx, globalObject, atomPrint, cPrint);
    const char *jscode = "print(\"hello, this is js invoke c print\")";
    JSValue jsResult = JS_Eval(ctx, jscode, strlen(jscode), "<js_code>", JS_EVAL_TYPE_GLOBAL);
    JS_FreeValue(ctx, jsResult);
    JS_FreeValue(ctx, globalObject);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}


static JSClassID testProxyClzId;

struct TestProxy {
    std::string aValue;
};

static void testClz_finalizer(JSRuntime *rt, JSValue val) {
    TestProxy *testObject = (TestProxy *) JS_GetOpaque(val, testProxyClzId);
    LOGE("TestProxy finalizer");
    if (testObject) {
        delete testObject;
    }
}

static JSValue
testProxy_get_property(JSContext *ctx, JSValueConst this_val, JSAtom prop, JSValueConst receiver) {
    const char *prop_name = JS_AtomToCString(ctx, prop);
    TestProxy *testObject = (TestProxy *) JS_GetOpaque(this_val, testProxyClzId);
    if (testObject == nullptr) {
        return JS_EXCEPTION;
    }
    if (strcmp(prop_name, "aValue") == 0) {
        return JS_NewString(ctx, testObject->aValue.c_str());
    }
    return JS_NewString(ctx, testObject->aValue.c_str());
}

static int
testProxy_set_property(JSContext *ctx, JSValueConst this_val, JSAtom prop, JSValueConst value,
                       JSValueConst receiver, int flags) {
    const char *prop_name = JS_AtomToCString(ctx, prop);
    TestProxy *testObject = (TestProxy *) JS_GetOpaque(this_val, testProxyClzId);
    if (testObject == nullptr) {
        return 0;
    }
    if (strcmp(prop_name, "aValue") == 0) {
        const char *str = JS_ToCString(ctx, value);
        if (str == nullptr) {
            return 0;
        }
        testObject->aValue = str;
        JS_FreeCString(ctx, str);
        JS_FreeCString(ctx, prop_name);
    }
    return 1;
}


static JSClassExoticMethods my_exotic = {
        .get_property = testProxy_get_property,
        .set_property = testProxy_set_property,
};

static JSClassDef testProxyClzDef = {
        .class_name = "TestProxy",
        .finalizer = testClz_finalizer,
        .exotic = &my_exotic
};

static JSValue
testProxy_new(JSContext *ctx, JSValueConst new_target, int argc, JSValueConst *argv) {
    TestProxy *testObject = new TestProxy();
    JSValue jsNewObject = JS_NewObjectClass(ctx, testProxyClzId);
    testObject->aValue = "a default test object aVal";
    JS_SetOpaque(jsNewObject, testObject);
    return jsNewObject;
}

extern "C"
JNIEXPORT void JNICALL
Java_com_example_quickjs_NativeLib_testJSProxyObject(JNIEnv *env, jobject thiz) {
    JSRuntime *rt = JS_NewRuntime();
    if (rt == nullptr) {
        return;
    }

    JSContext *ctx = JS_NewContext(rt);
    if (ctx == nullptr) {
        JS_FreeRuntime(rt);
        return;
    }

    JSValue globalObject = JS_GetGlobalObject(ctx);
    JSValue cPrint = JS_NewCFunction(ctx, js_print, "print", 1);
    JSAtom atomPrint = JS_NewAtom(ctx, "print");
    JS_SetProperty(ctx, globalObject, atomPrint, cPrint);

    JS_NewClassID(&testProxyClzId);
    JS_NewClass(rt, testProxyClzId, &testProxyClzDef);
    JSValue jsNewObject = JS_NewObject(ctx);
    JSValue jsTestProxyCtor = JS_NewCFunction2(ctx, testProxy_new, "TestProxy", 1,
                                               JS_CFUNC_constructor, 0);
    JS_SetConstructor(ctx, jsTestProxyCtor, jsNewObject);
    JS_SetClassProto(ctx, testProxyClzId, jsNewObject);
    JS_SetProperty(ctx, globalObject, JS_NewAtom(ctx, "TestProxy"), jsTestProxyCtor);


    const char *jscode = "print(\"哈哈哈，这里是JS调用C方法\");\n"
                         "let testProxy = new TestProxy();\n"
                         "print(testProxy.aValue);\n"
                         "testProxy.aValue = \"change new aValue\";\n"
                         "print(testProxy.aValue);";

    JSValue jsResult = JS_Eval(ctx, jscode, strlen(jscode), "<js_code>", JS_EVAL_TYPE_GLOBAL);
    JS_FreeValue(ctx, jsResult);
    JS_FreeValue(ctx, globalObject);
    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}