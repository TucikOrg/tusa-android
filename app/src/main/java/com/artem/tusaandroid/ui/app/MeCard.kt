package com.artem.tusaandroid.ui.app

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Card
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.requests.changeNameRequest
import com.artem.tusaandroid.ui.modal.InputNameModal
import com.artem.tusaandroid.ui.modal.InputPhoneModal
import com.artem.tusaandroid.ui.modal.InputSmsModal
import com.artem.tusaandroid.ui.modal.ProfileSettingsModal

@Composable
fun MeCard() {
    var phone by remember {
        mutableStateOf(AppVariables.getInstance().getPhone())
    }
    val name = remember {
        mutableStateOf(AppVariables.getInstance().getName())
    }
    val uniqueName = remember {
        mutableStateOf(AppVariables.getInstance().getUniqueName())
    }

    val showInputNameModal = remember { mutableStateOf(false) }
    if (showInputNameModal.value) {
        val coroutineScope = rememberCoroutineScope()
        InputNameModal(showModal = showInputNameModal) { inputName ->
            changeNameRequest(coroutineScope, inputName)
            showInputNameModal.value = false
            name.value = inputName
        }
    }

    var jwt by remember {
        mutableStateOf(AppVariables.getInstance().getJwt())
    }
    val showInputSms = remember { mutableStateOf(false) }
    if (showInputSms.value) {
        InputSmsModal(showModal = showInputSms, phone) { loginResult ->
            if (loginResult.isOk) {
                val loginResponse = loginResult.loginResponse!!
                jwt = loginResponse.jwt
                if (loginResponse.name.isEmpty()) {
                    showInputNameModal.value = true
                    showInputSms.value = false
                }
            }

        }
    }

    val showPhoneInput = remember { mutableStateOf(false) }
    if (showPhoneInput.value) {
        InputPhoneModal(showModal = showPhoneInput) { inputPhone, sent ->
            if (sent) {
                AppVariables.getInstance().savePhone(inputPhone)
                phone = inputPhone
                showPhoneInput.value = false
                showInputSms.value = true
            }
        }
    }

    val showProfileSettings = remember { mutableStateOf(false) }
    if (showProfileSettings.value) {
        ProfileSettingsModal(showProfileSettings, name, uniqueName)
    }
    Card(
        modifier = Modifier
            .padding(horizontal = 16.dp, vertical = 6.dp)
            .fillMaxWidth(),
        onClick = {
            if (AppVariables.getInstance().getJwt().isNotEmpty()) {
                showProfileSettings.value = true
            } else {
                showPhoneInput.value = true
            }
        }
    ) {
        Row(
            modifier = Modifier.padding(8.dp),
            verticalAlignment = Alignment.CenterVertically,
            horizontalArrangement = Arrangement.SpaceBetween
        ) {
            Column(
                modifier = Modifier.weight(1f),
                verticalArrangement = Arrangement.Center
            ) {
                val nameFilled = name.value.isNotEmpty()
                val phoneFilled = phone.isNotEmpty()
                if (nameFilled || phoneFilled) {
                    Text(
                        text = name.value.ifEmpty { phone },
                        style = MaterialTheme.typography.headlineSmall,
                        fontWeight = FontWeight.Bold
                    )
                    Spacer(modifier = Modifier.height(4.dp))
                }
                Row(
                    modifier = Modifier.fillMaxWidth()
                ) {
                    if (phoneFilled && nameFilled) {
                        Text(
                            text = phone,
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Gray
                        )
                        Spacer(modifier = Modifier.width(11.dp))
                    }
                    if (uniqueName.value.isNotEmpty()) {
                        Text(
                            text = "@" + uniqueName.value,
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Gray
                        )
                    }
                }

                if (!phoneFilled) {
                    TextButton(
                        onClick = {
                            showPhoneInput.value = true
                        }
                    ) {
                        Text(
                            text = "Войти в аккаунт",
                            fontWeight = FontWeight.Bold,
                            fontSize = MaterialTheme.typography.headlineSmall.fontSize
                        )
                    }
                }
            }

            Avatar()
        }
    }
}




