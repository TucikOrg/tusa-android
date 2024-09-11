package com.artem.tusaandroid.app.login

import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.app.action.MainActionFabViewModel
import com.artem.tusaandroid.phone.ru.InputRuPhoneNumber

@Composable
fun InputPhone(model: InputPhoneViewModel, rootModel: MainActionFabViewModel) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(horizontal = 10.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            text = "Укажите телефон",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold
        )

        Text(
            text = "Мы отправим вам смс с кодом для входа",
        )

        Spacer(modifier = Modifier.height(40.dp))
        InputRuPhoneNumber(30.0f) { inputPhone ->
            model.sendCodeToPhone(inputPhone, rootModel)
        }
        Spacer(modifier = Modifier.height(11.dp))

        val context = LocalContext.current
        Text(
            text = model.legalDocumentsAnnotated,
            style = MaterialTheme.typography.bodySmall,
            color = Color.Gray,
            textAlign = TextAlign.Center,
            modifier = Modifier.clickable {
                model.openLegalDocumentsInBrowser(context)
            }
        )
        Spacer(modifier = Modifier.height(40.dp))
    }
}