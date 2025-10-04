package com.example.quickjs

import android.content.Context
import com.example.quickjs.bridge.JSBridge

class JSContext(val context: Context) {
    lateinit var jsRuntime: JSRuntime


    init {
        jsRuntime = JSRuntime().apply { initRuntime() }
        registerGlobalFun()
        registerGlobalObj()
    }

    private fun registerGlobalObj() {
        jsRuntime.setGlobalObj("JSBridge", JSBridge(jsRuntime, this).ptr)
    }

    private fun registerGlobalFun() {
        jsRuntime.setGlobalFun("markStart", JSFunction(jsRuntime, object : JSFunction.JavaCallback {
            override fun call(args: Array<Any?>): Any? {
                return "android received markStart${args.joinToString(",")}, and return this str"
            }
        }).ptr)
    }
}