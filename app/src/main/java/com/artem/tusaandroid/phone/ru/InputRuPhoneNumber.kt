package com.artem.tusaandroid.phone.ru

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.BasicTextField
import androidx.compose.foundation.text.KeyboardOptions
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.focus.FocusRequester
import androidx.compose.ui.focus.focusRequester
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.input.ImeAction
import androidx.compose.ui.text.input.KeyboardType
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.text.isDigitsOnly
import com.artem.tusaandroid.app.CharView

@Composable
fun InputRuPhoneNumber(size: Float, onDone: (String) -> Unit){
    val focusRequester = FocusRequester()
    LaunchedEffect(Unit) {
        focusRequester.requestFocus()
    }
    val phoneNumber = remember { mutableStateOf("") }
    val currentInputIndex = phoneNumber.value.lastIndex
    BasicTextField(
        modifier = Modifier.focusRequester(focusRequester),
        value = phoneNumber.value,
        singleLine = true,
        onValueChange = {
            if (!it.isDigitsOnly()) {
                return@BasicTextField
            }
            phoneNumber.value = it
            if (phoneNumber.value.length > 9) {
                onDone.invoke("+7" + phoneNumber.value)
            }
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
                Text("+7",
                    modifier = Modifier
                        .padding(vertical = 4.dp, horizontal = 4.dp),
                    fontWeight = FontWeight.Bold,
                    fontSize = MaterialTheme.typography.headlineSmall.fontSize
                )

                repeat(10) { index ->
                    var char = " "
                    if (phoneNumber.value.length > index) {
                        char = phoneNumber.value[index].toString()
                    }
                    if (index == 3 || index == 6 || index == 8) {
                        Text("-",
                            modifier = Modifier
                                .padding(vertical = 4.dp, horizontal = 1.dp),
                            fontWeight = FontWeight.Bold,
                            fontSize = MaterialTheme.typography.headlineSmall.fontSize
                        )
                    }
                    CharView(
                        selected = currentInputIndex + 1 == index,
                        text = char,
                        size = size
                    )
                }
            }
        }
    )
}

@Preview(showBackground = true)
@Composable
fun RuPhonePreview() {
    return InputRuPhoneNumber(30.0f) {

    }
}