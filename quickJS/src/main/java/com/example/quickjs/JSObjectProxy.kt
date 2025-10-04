package com.example.quickjs

import androidx.annotation.Keep

@Keep
open class JSObjectProxy(
    protected val jsRuntime: JSRuntime
) {

    val ptr: Long by lazy { createNativeIns() }

    open fun createNativeIns(): Long = jsRuntime.createJSObject(this)

    open fun getter(propName: String): Any? = null

    open fun setter(propName: String, value: Any?) {

    }

    fun addRefCount() {
        jsRuntime.addRefCount(ptr)
    }

    fun releaseRefCount() {
        jsRuntime.releaseRefCount(ptr)
    }

    fun destroy() {
        jsRuntime.releaseObj(ptr)
    }
}