package com.example.ggjs

import android.content.Context
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Button
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
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
    val person by remember { mutableStateOf(Person(jsContext.value.jsRuntime)) }

    val evalCode = remember {  jsContext.value.jsRuntime.evalCode(
        "result = markStart(1, \"hello\", true);\n" +
                "print(result);\n" +
                "print(\"开始执行一段js代码并且返回一个int值\");\n" +
                "a=1;\n" +
                "b=2;\n" +
                "a+b;", "GGJS"
    ) }

    Column(
        modifier = Modifier.fillMaxSize(),
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.Center
    ) {
        Text(
            text = "Hello $name!，now invoke a static js code and a + b result is $evalCode",
            modifier = modifier
        )
        Text(
            text = "Hello ${person.name}, age is ${person.age}",
            modifier = Modifier.padding(top = 16.dp)
        )
        Row(
            horizontalArrangement = Arrangement.spacedBy(8.dp),
            verticalAlignment = Alignment.CenterVertically
        ) {
            Button(
                modifier = Modifier
                    .weight(1f)
                    .padding(16.dp),
                onClick = {
                    jsContext.value.jsRuntime.evalCode(
                        "JSBridge.call(\"toast\", \"今年我${person.age}岁了\");", "GGJS"
                    )
                }
            ) {
                Text(text = "点我Toast")
            }
            Button(
                modifier = Modifier
                    .weight(1F)
                    .padding(16.dp),
                onClick = {
                    jsContext.value.jsRuntime.evalCode(
                        "person.age = person.age + 1;" +
                                "print(\"now age is \" + person.age);", "GGJS"
                    )
                }
            ) {
                Text(text = "点我增加年龄")
            }
        }
    }
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    GGJSTheme {
        Greeting(LocalContext.current, "Android")
    }
}