package com.artem.tusaandroid.ui.modal

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.requests.requestCodeToPhone
import com.artem.tusaandroid.ui.phone.ru.InputRuPhoneNumber

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun InputPhoneModal(showModal: MutableState<Boolean>, onDone: (String, Boolean) -> Unit) {
    ModalBottomSheet(
        onDismissRequest = { showModal.value = false }
    ) {
        val coroutineScope = rememberCoroutineScope()
        var errorText by remember { mutableStateOf("") }
        if (errorText.isNotEmpty()) {
            Text(
                text = errorText,
                style = MaterialTheme.typography.bodyLarge,
                fontWeight = FontWeight.Bold,
                color = Color.Red,
                modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
            )
        }

        InputRuPhoneNumber { inputPhone ->
            requestCodeToPhone(coroutineScope, inputPhone) { response ->
                val code = response.code
                if (code != 200) {
                    errorText = "Не можем отправить код сейчас. Попробуйте позже или напишите в поддержку"
                }
                onDone.invoke(inputPhone, code == 200)
            }
        }
        Spacer(modifier = Modifier.height(21.dp))
    }
}

@Composable
@Preview
fun InputPhoneModalPreview() {
    val showModal = remember { mutableStateOf(true) }
    InputPhoneModal(showModal) { phone, sent ->

    }
}