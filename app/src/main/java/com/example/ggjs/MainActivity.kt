package com.example.ggjs

import android.content.Context
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import com.example.ggjs.ui.theme.GGJSTheme
import com.example.quickjs.JSContext

class MainActivity : ComponentActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        enableEdgeToEdge()
        setContent {
            GGJSTheme {
                Scaffold(modifier = Modifier.fillMaxSize()) { innerPadding ->
                    Greeting(
                        this,
                        name = "Android",
                        modifier = Modifier.padding(innerPadding)
                    )
                }
            }
        }
    }
}

@Composable
fun Greeting(context: Context, name: String, modifier: Modifier = Modifier) {
    val jsContext = remember { mutableStateOf(JSContext(context)) }
    val evalCode = jsContext.value.jsRuntime.evalCode(
        "print(\"开始执行一段js代码并且返回一个int值\");\n" +
                "a=1;\n" +
                "b=2;\n" +
                "a+b;", "GGJS"
    )
    Text(
        text = "Hello $name!，invoke js and result is $evalCode",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    GGJSTheme {
        Greeting(LocalContext.current, "Android")
    }
}