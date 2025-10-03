package com.example.quickjs

import android.content.Context

class JSContext(val context: Context) {
    lateinit var jsRuntime: JSRuntime


    init {
        jsRuntime = JSRuntime().apply { initRuntime() }
        registerGlobalFun()
        registerGlobalObj()
    }

    private fun registerGlobalObj() {

    }

    private fun registerGlobalFun() {

    }
}