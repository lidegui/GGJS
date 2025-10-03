//
// Created by Administrator on 2025/10/3.
//

#ifndef GGJS_JSI_H
#define GGJS_JSI_H

#include <__functional/function.h>
#include "quickjs/quickjs.h"
#include "string"
#include "utils.h"
#include "jni.h"
#include "set"

using namespace std;

class GGRuntime {
public:
    jobject javaRuntime;
    JSRuntime *rt;
    set<jlong> objFunctionCache;

    GGRuntime() {};

    void init();

    JSValue eval(const char *code, const char *fileName);

    JSContext *getCtx();

    void destroy();

private:
    JSContext *ctx;

    void registerClass();

    void registerGlobalFun();
};

class JSIObject {
public:
    JSIObject() {}

    JSValue get(std::string propName);

    void set(std::string propNme, JSValue value);

    void addRefCount() {
        JS_DupValue(ctx, js_value);
    }

    void releaseRefCount() {
        JS_FreeValue(ctx, js_value);
    }

    void onDestroy();

    ~JSIObject() { onDestroy(); };

    static JSValue wrap(GGRuntime *rt, JSIObject *jsiObject);

protected:
    JSContext *ctx;
    GGRuntime *rt;
    JSValue js_value;
    unordered_map<string, jlong> functionCache;
};

static JSClassID jsi_class_id;

static void jsi_obj_finalizer(JSRuntime *rt, JSValue val) {
    JSIObject *jsiObject = (JSIObject *) JS_GetOpaque(val, jsi_class_id);
    if (jsiObject) {
        LOGE("delete jsiObject");
        delete jsiObject;
    }
}


static JSValue jsi_get_property(JSContext *ctx, JSValueConst obj, JSAtom atom,
                                JSValueConst receiver) {
    JSIObject *jsiObject = (JSIObject *) JS_GetOpaque(obj, jsi_class_id);
    const char *propName = JS_AtomToCString(ctx, atom);
    JSValue jsValue = jsiObject->get(propName);
    JS_FreeCString(ctx, propName);
    return jsValue;
};

static int jsi_set_property(JSContext *ctx, JSValueConst obj, JSAtom atom,
                            JSValueConst value, JSValueConst receiver, int flags) {
    JSIObject *jsiObject = (JSIObject *) JS_GetOpaque(obj, jsi_class_id);
    const char *propName = JS_AtomToCString(ctx, atom);
    jsiObject->set(propName, value);
    JS_FreeCString(ctx, propName);
    return 1;
};

static JSClassExoticMethods jsi_exotic{
        .get_property = jsi_get_property,
        .set_property = jsi_set_property

};

static JSClassDef jsi_obj_class = {
        .class_name = "JSIObject",
        .finalizer = jsi_obj_finalizer,
        .exotic = &jsi_exotic,
};

// 自定义类的id
static JSClassID fun_class_id;

using JSCallback = std::function<JSValue(JSContext *ctx, JSValueConst func_obj,
                                         JSValue this_val, int argc, JSValueConst *argv,
                                         int flags)>;

static void jsi_fun_finalizer(JSRuntime *rt, JSValue val) {
    JSCallback *callback = (JSCallback *) JS_GetOpaque(val, fun_class_id);
    if (callback) {
        LOGE("delete fun callback");
        delete callback;
    }
}


static JSValue js_callback_adapter(JSContext *ctx, JSValueConst func_obj,
                                   JSValueConst this_val, int argc, JSValueConst *argv,
                                   int flags) {
    JSCallback *fn = (JSCallback *) JS_GetOpaque(func_obj, fun_class_id);
    if (fn) {
        return (*fn)(ctx, func_obj, this_val, argc, argv, flags);
    }

    return JS_EXCEPTION;
};

static JSClassDef jsi_fun_class = {
        .class_name = "JSIFunction",
        .finalizer = jsi_fun_finalizer,
        .call = js_callback_adapter
};

JSValue createJSIFunction(JSContext *ctx, JSCallback callback);

JSIObject *getJSIObject(JSContext *ctx, JSValue value);

#endif //GGJS_JSI_H
