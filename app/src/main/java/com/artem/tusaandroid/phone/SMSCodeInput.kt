package com.artem.tusaandroid.phone

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.tooling.preview.Preview
import androidx.core.text.isDigitsOnly
import com.artem.tusaandroid.app.CharView

@Composable
fun SMSCodeInput(
    onCodeComplete: (String) -> Unit,
) {
    val focusRequester = FocusRequester()
    var code by remember { mutableStateOf("") }
    val currentInputIndex = code.lastIndex
    LaunchedEffect(Unit) {
        focusRequester.requestFocus()
    }
    BasicTextField(
        modifier = Modifier.focusRequester(focusRequester).fillMaxWidth(),
        value = code,
        singleLine = true,
        onValueChange = {
            if (!it.isDigitsOnly()) {
                return@BasicTextField
            }
            if (it.length > 4) {
                return@BasicTextField
            }
            if (it.length == 4) {
                code = it
                onCodeComplete(code)
                return@BasicTextField
            }
            code = it
        },
        enabled = true,
        keyboardOptions = KeyboardOptions(
            keyboardType = KeyboardType.Number,
            imeAction = ImeAction.Next
        ),
        decorationBox = {
            Row(
                modifier = Modifier.fillMaxWidth(),
                horizontalArrangement = Arrangement.Center,
                verticalAlignment = Alignment.CenterVertically
            ) {
                repeat(4) { index ->
                    var char = " "
                    if (code.length > index) {
                        char = code[index].toString()
                    }
                    CharView(
                        selected = currentInputIndex + 1 == index,
                        text = char,
                        size = 30.0f
                    )
                }
            }
        }
    )
}


@Composable
@Preview
fun SMSCodeInputPreview() {
    SMSCodeInput() { }
}