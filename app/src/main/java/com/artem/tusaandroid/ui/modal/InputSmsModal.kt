package com.artem.tusaandroid.ui.modal

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableIntStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.dto.LoginResponseDto
import com.artem.tusaandroid.requests.LoginRequestResult
import com.artem.tusaandroid.requests.requestCodeToPhone
import com.artem.tusaandroid.requests.sendLoginRequest
import com.artem.tusaandroid.ui.phone.SMSCodeInput
import kotlinx.coroutines.delay

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun InputSmsModal(showModal: MutableState<Boolean>, phone: String, onDone: (LoginRequestResult) -> Unit) {
    ModalBottomSheet(
        onDismissRequest = { showModal.value = false }
    ) {
        val coroutineScope = rememberCoroutineScope()
        Column(
            modifier = Modifier.fillMaxWidth(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            var errorText by remember {
                mutableStateOf("")
            }
            if (errorText.isNotEmpty()) {
                Text(
                    text = errorText,
                    color = MaterialTheme.colorScheme.error,
                    textAlign = TextAlign.Center,
                    fontWeight = FontWeight.Bold,
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 1.dp)
                )
            }

            Row(
                modifier = Modifier.padding(vertical = 0.dp)
            ) {
                Text(
                    text = "Код отправлен на номер:",
                    style = MaterialTheme.typography.bodyMedium,
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                )
                Text(
                    text = phone,
                    style = MaterialTheme.typography.bodyMedium,
                    color = MaterialTheme.colorScheme.primary,
                    modifier = Modifier.padding(horizontal = 10.dp, vertical = 6.dp)
                )
            }

            var timeLeft by remember { mutableStateOf(10) }
            LaunchedEffect(timeLeft) {
                if (timeLeft <= 0) {
                    return@LaunchedEffect
                }
                delay(1000L)
                timeLeft -= 1
            }

            TextButton(
                modifier = Modifier.padding(vertical = 0.dp),
                onClick = {
                    if (timeLeft <= 0) {
                        requestCodeToPhone(coroutineScope, phone) { response ->
                            if (response.code != 200) {
                                errorText = "Не можем отправить код сейчас. Попробуйте позже или напишите в поддержку"
                                return@requestCodeToPhone
                            }
                            timeLeft = 10
                        }
                        return@TextButton
                    }
                    timeLeft = 10
                }
            ) {
                Text(
                    text = if (timeLeft > 0) "Можно отправить повторно через $timeLeft секунд" else "Отправить повторно",
                    modifier = Modifier.padding(0.dp)
                )
            }

            val codeInvalid = remember { mutableStateOf(false) }
            SMSCodeInput(codeInvalid) { code ->
                sendLoginRequest(coroutineScope, code) { result ->
                    if (result.isOk) {
                        val loginResponse = result.loginResponse!!
                        AppVariables.getInstance().saveJwt(loginResponse.jwt)

                        if (loginResponse.name.isNotEmpty()) {
                            AppVariables.getInstance().saveName(loginResponse.name)
                        }
                        if (loginResponse.uniqueName.isNotEmpty()) {
                            AppVariables.getInstance().saveUniqueName(loginResponse.uniqueName)
                        }
                    }

                    var errorTextSet = "Не получается войти сейчас. Попробуйте позже или напишите в поддержку"
                    if (result.isCodeInvalid) {
                        errorTextSet = "Неверный код"
                        codeInvalid.value = true
                    }

                    if (result.isOk.not()) {
                        errorText = errorTextSet
                        codeInvalid.value = true
                    }

                    onDone.invoke(result)
                }
            }
        }
    }
}

@Composable
@Preview(showBackground = true)
fun showInputSmsModalPreview() {
    val showModal = remember { mutableStateOf(true) }
    InputSmsModal(showModal, "+79999999999") { result ->

    }
}