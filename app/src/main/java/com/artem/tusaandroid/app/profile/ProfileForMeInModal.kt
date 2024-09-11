package com.artem.tusaandroid.app.profile

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@Preview
@Composable
fun ProfileForMeInModalPreview() {
    ProfileForMeInModal(PreviewProfileCardViewModel())
}

@Composable
fun ProfileForMeInModal(model: ProfileCardViewModel) {
    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 10.dp),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            modifier = Modifier.fillMaxWidth(),
            text = "Профиль",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold,
            textAlign = TextAlign.Start
        )
        Spacer(modifier = Modifier.height(21.dp))
        val keyboard = LocalSoftwareKeyboardController.current
        OutlinedTextField(
            value = model.name?: "",
            onValueChange = {
                model.name = it
            },
            placeholder = {
                Text("Введите ваше имя",
                    textAlign = TextAlign.Center,
                    modifier = Modifier.fillMaxWidth()
                )
            },
            keyboardActions = KeyboardActions(
                onDone = {
                    model.saveName()
                    keyboard?.hide()
                }
            ),
            singleLine = true,
            enabled = true,
            textStyle = MaterialTheme.typography.bodyMedium.copy(textAlign = TextAlign.Center),
            modifier = Modifier
                .fillMaxWidth()
        )
        Spacer(modifier = Modifier.height(40.dp))
    }
}


