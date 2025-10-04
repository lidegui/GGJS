package com.example.ggjs

import androidx.compose.runtime.mutableStateOf
import com.example.quickjs.JSObjectProxy
import com.example.quickjs.JSRuntime

class Person(jsRuntime: JSRuntime) : JSObjectProxy(jsRuntime) {
    private val _name = mutableStateOf("龟龟")
    private val _age = mutableStateOf(18)

    var name: String
        get() = _name.value
        set(value) {
            _name.value = value
        }

    var age: Int
        get() = _age.value
        set(value) {
            _age.value = value
        }

    init {
        jsRuntime.setGlobalObj("person", ptr)
    }

    override fun getter(propName: String): Any? {
        when (propName) {
            "name" -> return name
            "age" -> return age
        }
        return super.getter(propName)
    }

    override fun setter(propName: String, value: Any?) {
        when (propName) {
            "name" -> (value as? String)?.let { name = it }
            "age" -> (value as? Int)?.let { age = it }
        }
    }

}