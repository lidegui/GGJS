package com.example.quickjs

class NativeLib {

    /**
     * A native method that is implemented by the 'quickjs' native library,
     * which is packaged with this application.
     */
    external fun stringFromJNI(): String

    companion object {
        // Used to load the 'quickjs' library on application startup.
        init {
            System.loadLibrary("quickjs")
        }
    }
}