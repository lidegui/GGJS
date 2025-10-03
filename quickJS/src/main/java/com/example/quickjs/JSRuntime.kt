package com.example.quickjs

class JSRuntime {
    var runtimePtr: Long = 0


    fun propGetter(ptr: Long, propName: String): Any? {
        return null
    }

    fun propSetter(ptr: Long, propName: String, value: Any?) {

    }

    fun methodCall(methodId: Long, args: Array<Any?>): Any? {
        return null
    }

    external fun nativeInitRuntime(): Long

    external fun nativeEvalCode(ptr: Long, code: String, fileName: String): Any

    fun initRuntime() {
        runtimePtr = nativeInitRuntime()
    }

    fun evalCode(code: String, fileName: String): Any {
        return nativeEvalCode(runtimePtr, code, fileName)
    }
}