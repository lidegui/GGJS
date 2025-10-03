//
// Created by Administrator on 2025/10/3.
//

#include "jsi.h"

JSValue JSIObject::wrap(GGRuntime *rt, JSIObject *jsiObject) {
    if (!jsiObject) {
        return JS_UNDEFINED;
    }

    JSValue jsValue = JS_NewObjectClass(rt->getCtx(), jsi_class_id);
    JS_SetOpaque(jsValue, jsiObject);
    jsiObject->rt = rt;
    jsiObject->js_value = jsValue;
    jsiObject->ctx = rt->getCtx();
    return jsValue;
}

void JSIObject::onDestroy() {

}

void GGRuntime::init() {
    rt = JS_NewRuntime();
    ctx = JS_NewContext(rt);
    registerClass();
    registerGlobalFun();
}

void GGRuntime::registerClass() {
    JS_NewClassID(&jsi_class_id);
    JS_NewClass(rt, jsi_class_id, &jsi_obj_class);
    JS_NewClassID(&fun_class_id);
    JS_NewClass(rt, fun_class_id, &jsi_fun_class);
}

JSValue GGRuntime::eval(const char *code, const char *fileName) {
    return JS_Eval(ctx, code, strlen(code), fileName, JS_EVAL_TYPE_GLOBAL);
}

JSContext *GGRuntime::getCtx() {
    return ctx;
}

void GGRuntime::destroy() {
    for (jlong funPtr: objFunctionCache) {
        JSValue val = JS_MKPTR(JS_TAG_OBJECT, reinterpret_cast<void *>(funPtr));
        LOGE("destroy function");
        if (JS_IsFunction(ctx, val)) {
            LOGE("free js function");
            JS_FreeValue(ctx, val);
        }
    }

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
}

JSValue createJSIFunction(JSContext *ctx, JSCallback callback) {
    JSValue jsiFunc = JS_NewObjectClass(ctx, fun_class_id);
    JSCallback *cb_ptr = new JSCallback(std::move(callback));
    JS_SetOpaque(jsiFunc, cb_ptr);
    return jsiFunc;
}