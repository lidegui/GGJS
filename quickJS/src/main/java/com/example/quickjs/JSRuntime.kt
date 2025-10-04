package com.example.quickjs

class JSRuntime {
    var runtimePtr: Long = 0

    val trackProxyMap = mutableMapOf<Long, JSObjectProxy>()
    val trackMethodMap = mutableMapOf<Long, JSFunction>()

    fun propGetter(ptr: Long, propName: String): Any? {
        return trackProxyMap[ptr]?.getter(propName)
    }

    fun propSetter(ptr: Long, propName: String, value: Any?) {
        trackProxyMap[ptr]?.setter(propName, value)
    }

    fun methodCall(methodId: Long, args: Array<Any?>): Any? {
        return trackMethodMap[methodId]?.call(args)
    }

    fun initRuntime() {
        runtimePtr = nativeInitRuntime()
    }

    fun evalCode(code: String, fileName: String): Any {
        return nativeEvalCode(runtimePtr, code, fileName)
    }

    fun isJSProxyObject(ptr: Long): Boolean {
        return trackProxyMap.containsKey(ptr)
    }

    fun getProxyObject(ptr: Long): Any? {
        return trackProxyMap[ptr]
    }

    fun createJSFunction(jsFunction: JSFunction): Long {
        val jsFunctionPtr = nativeCreateJSFunction(runtimePtr)
        trackMethodMap[jsFunctionPtr] = jsFunction
        return jsFunctionPtr
    }

    fun createJSObject(jsObject: JSObjectProxy): Long {
        val jsObjectPtr = nativeCreateJSObj(runtimePtr)
        trackProxyMap[jsObjectPtr] = jsObject
        return jsObjectPtr
    }

    fun setGlobalFun(functionName: String, callbackPtr: Long) {
        nativeSetGlobalFun(runtimePtr, functionName, callbackPtr)
    }

    fun setGlobalObj(objectName: String, objectPtr: Long) {
        nativeSetGlobalObj(runtimePtr, objectName, objectPtr)
    }

    fun addRefCount(objPtr: Long) {
        addRefCount(runtimePtr, objPtr)
    }

    fun releaseRefCount(objPtr: Long) {
        releaseRefCount(runtimePtr, objPtr)
    }

    fun releaseObj(objPtr: Long) {
        nativeReleaseObj(runtimePtr, objPtr)
    }

    fun destroy() {
        nativeDestroy(runtimePtr)
        trackProxyMap.clear()
    }

    external fun nativeInitRuntime(): Long

    external fun nativeCreateJSObj(ptr: Long): Long

    external fun nativeCreateJSFunction(ptr: Long): Long

    external fun nativeEvalCode(ptr: Long, code: String, fileName: String): Any

    external fun nativeSetGlobalFun(ptr: Long, functionName: String, callbackPtr: Long)

    external fun nativeSetGlobalObj(ptr: Long, objectName: String, objectPtr: Long)

    external fun nativeDestroy(ptr: Long)

    external fun addRefCount(runtimePtr: Long, objPtr: Long)

    external fun releaseRefCount(runtimePtr: Long, objPtr: Long)

    external fun nativeReleaseObj(runtimePtr: Long, objPtr: Long)
}