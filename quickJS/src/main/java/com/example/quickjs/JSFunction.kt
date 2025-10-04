package com.example.quickjs

open class JSFunction(
    jsRuntime: JSRuntime,
    private val callback: JavaCallback
) : JSObjectProxy(jsRuntime) {


    override fun createNativeIns(): Long {
        return jsRuntime.createJSFunction(this)
    }

    fun call(args: Array<Any?>): Any? {
        return callback.call(args)
    }


    interface JavaCallback {
        fun call(args: Array<Any?>): Any?
    }
}