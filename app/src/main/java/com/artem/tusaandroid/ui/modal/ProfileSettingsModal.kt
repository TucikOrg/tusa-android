package com.artem.tusaandroid.ui.modal

import androidx.compose.foundation.layout.Column
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
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.requests.changeNameRequest
import com.artem.tusaandroid.requests.changeUniqueNameRequest

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ProfileSettingsModal(showModal: MutableState<Boolean>, name: MutableState<String>, uniqueName: MutableState<String>) {
    ModalBottomSheet(
        onDismissRequest = { showModal.value = false }
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 10.dp)
        ) {
            Text(text = "Профиль",
                style = MaterialTheme.typography.headlineLarge,
                modifier = Modifier.padding(bottom = 4.dp)
            )
            val keyboardController = LocalSoftwareKeyboardController.current
            val coroutineScope = rememberCoroutineScope()
            OutlinedTextField(
                value = name.value,
                onValueChange = {
                    name.value = it
                },
                modifier = Modifier
                    .fillMaxWidth(),
                label = {
                    Text("Имя")
                },
                placeholder = {
                    Text("Введите ваше имя",
                        textAlign = TextAlign.Center,
                        modifier = Modifier.fillMaxWidth()
                    )
                },
                keyboardActions = KeyboardActions(
                    onDone = {
                        if (AppVariables.getInstance().nameOrEmpty() != name.value && name.value.isEmpty().not()) {
                            changeNameRequest(coroutineScope, name.value)
                        }
                        keyboardController?.hide()
                    }
                ),
                singleLine = true,
                enabled = true,
                textStyle = MaterialTheme.typography.headlineSmall.copy(textAlign = TextAlign.Center),
            )
            Row(
                modifier = Modifier.fillMaxWidth().padding(vertical = 10.dp),
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(
                    text = "@",
                    style = MaterialTheme.typography.headlineMedium.copy(fontWeight = FontWeight.Bold),
                    modifier = Modifier.padding(horizontal = 6.dp)
                )
                var errorText by remember { mutableStateOf("") }
                var isError by remember { mutableStateOf(false) }
                Column(
                    modifier = Modifier.fillMaxWidth(),
                ) {
                    OutlinedTextField(
                        modifier = Modifier.fillMaxWidth(),
                        value = uniqueName.value?: "",
                        onValueChange = {
                            uniqueName.value = it
                            val currentUniqueName = AppVariables.getInstance().getUniqueName()
                            if (currentUniqueName != uniqueName.value && uniqueName.value.isEmpty().not()) {
                                changeUniqueNameRequest(coroutineScope, uniqueName.value)  { result ->
                                    if (result.isOk) {
                                        isError = result.saved.not()
                                        errorText = "Никнейм занят"
                                    }
                                }
                            }
                        },
                        isError = isError,
                        label = {
                            Text("Никнейм")
                        },
                        singleLine = true,
                        enabled = true,
                        textStyle = MaterialTheme.typography.headlineSmall.copy(textAlign = TextAlign.Center),
                        placeholder = {
                            Text(
                                "Никнейм",
                                textAlign = TextAlign.Center,
                                modifier = Modifier.fillMaxWidth()
                            )
                        }
                    )
                    if (errorText.isNotEmpty()) {
                        Text(
                            text = errorText,
                            style = MaterialTheme.typography.bodyLarge,
                            fontWeight = FontWeight.Bold,
                            color = MaterialTheme.colorScheme.error,
                            modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                        )
                    }
                }
            }
            Spacer(modifier = Modifier.height(40.dp))
        }
    }
}

@Composable
@Preview(showBackground = true)
fun ProfileSettingsModalPreview() {
    val showModal = remember { mutableStateOf(true) }
    val name = remember { mutableStateOf<String>("") }
    val uniqueName = remember { mutableStateOf<String>("") }
    ProfileSettingsModal(showModal, name, uniqueName)
}