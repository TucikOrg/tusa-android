package com.artem.tusaandroid.app.login

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.phone.SMSCodeInput
import kotlinx.coroutines.delay

@Composable
fun InputSMS(model: InputSMSViewModel, rootModel: MainActionFabViewModel) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(horizontal = 10.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            text = "Теперь введите код",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold
        )
        Spacer(modifier = Modifier.height(10.dp))
        Text(
            text = "Мы отправили на номер",
        )
        Text(
            text = model.getPhone().value,
        )

        LaunchedEffect(model.timeLeft) {
            if (model.timeLeft <= 0) {
                return@LaunchedEffect
            }
            delay(1000L)
            model.timeLeft -= 1
        }

        TextButton(
            modifier = Modifier.padding(vertical = 0.dp),
            onClick = {
                model.sendCodeToPhone(model.getPhone().value)
            }
        ) {
            Text(
                text = model.makeTimeLeftText(),
                modifier = Modifier.padding(0.dp)
            )
        }
        SMSCodeInput { code ->
            val phone = model.getPhone().value
            model.login(phone = phone, code = code, mainActionFabViewModel = rootModel)
        }
        Spacer(modifier = Modifier.height(40.dp))
    }
}