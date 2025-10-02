package com.example.quickjs

import android.util.Log

class NativeLib {

    /**
     * A native method that is implemented by the 'quickjs' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    external fun testQuickJS(): String

    external fun testJsInvokeC()

    external fun testJSProxyObject()

    companion object {
        // Used to load the 'quickjs' library on application startup.
        init {
            System.loadLibrary("quickjs")
            val nativeLib = NativeLib()
            val stringFromJNI = nativeLib.stringFromJNI()
            Log.i("ldg-test", ": stringFromJNI = $stringFromJNI")
            val testQuickJS = nativeLib.testQuickJS()
            Log.i("ldg-test", ": testQuickJS = $testQuickJS")
            nativeLib.testJsInvokeC()
            nativeLib.testJSProxyObject()
        }
    }
}