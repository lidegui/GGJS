package com.example.quickjs.bridge

import android.os.Handler
import android.os.Looper
import android.util.Log
import android.widget.Toast
import com.example.quickjs.JSContext
import com.example.quickjs.JSFunction
import com.example.quickjs.JSObjectProxy
import com.example.quickjs.JSRuntime

class JSBridge(
    jsRuntime: JSRuntime,
    private val jsJSContext: JSContext
) : JSObjectProxy(jsRuntime) {

    private val tag = "ldg-test-JSBridge"

    override fun getter(propName: String): Any? {
        when (propName) {
            "call" -> return JSFunction(jsRuntime, object : JSFunction.JavaCallback {
                override fun call(args: Array<Any?>): Any? {
                    Log.i(tag, "call args = ${args.joinToString()}")

                    val methodName = args[0] as? String ?: return null
                    when (methodName) {
                        "toast" -> {
                            val message = args[1] as? String ?: return null
                            Handler(Looper.getMainLooper()).post {
                                Toast.makeText(jsJSContext.context, message, Toast.LENGTH_SHORT)
                                    .show()
                                Log.i(tag, "toast message = $message")
                            }
                        }
                    }

                    return null
                }
            })
        }

        return super.getter(propName)
    }
}