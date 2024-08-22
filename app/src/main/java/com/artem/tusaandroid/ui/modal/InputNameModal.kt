package com.artem.tusaandroid.ui.modal

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun InputNameModal(showModal: MutableState<Boolean>, onDone: (String) -> Unit) {
    ModalBottomSheet(
        onDismissRequest = { showModal.value = false }
    ) {
        var name by remember {
            mutableStateOf("")
        }
        OutlinedTextField(
            value = name,
            onValueChange = {
                name = it
            },
            placeholder = {
                Text("Введите ваше имя",
                    textAlign = TextAlign.Center,
                    modifier = Modifier.fillMaxWidth()
                )
            },
            keyboardActions = KeyboardActions(
                onDone = {
                    onDone.invoke(name)
                }
            ),
            singleLine = true,
            enabled = true,
            textStyle = MaterialTheme.typography.headlineSmall.copy(textAlign = TextAlign.Center),
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 10.dp, vertical = 5.dp)
        )
        Spacer(modifier = Modifier.height(21.dp))
    }
}

@Preview(showBackground = true)
@Composable
fun InputNameModalPreview() {
    val showModal = remember {
        mutableStateOf(false)
    }
    InputNameModal(showModal = showModal) {

    }
}