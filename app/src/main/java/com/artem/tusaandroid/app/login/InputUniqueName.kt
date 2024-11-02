package com.artem.tusaandroid.app.login

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.app.action.MainActionFabViewModel

@Composable
fun InputUniqueName(model: InputUniqueNameViewModel, rootModel: MainActionFabViewModel) {
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(horizontal = 10.dp)
            .padding(bottom = 10.dp)
            .imePadding(),
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Text(
            text = "Уникальное имя пользователя",
            style = MaterialTheme.typography.headlineSmall,
            fontWeight = FontWeight.Bold
        )
        Spacer(modifier = Modifier.height(21.dp))
        OutlinedTextField(
            value = model.getUniqueName().value,
            onValueChange = {
                model.getUniqueName().value = it
            },
            label = {
                Text("@")
            },
            placeholder = {
                Text("никнейм",
                    textAlign = TextAlign.Center,
                    modifier = Modifier.fillMaxWidth()
                )
            },
            keyboardActions = KeyboardActions(
                onDone = {
                    model.changeUniqueName(model.getUniqueName().value, rootModel)
                }
            ),
            singleLine = true,
            enabled = true,
            textStyle = MaterialTheme.typography.headlineSmall.copy(textAlign = TextAlign.Center),
            modifier = Modifier
                .fillMaxWidth()
                .padding(horizontal = 10.dp, vertical = 5.dp)
        )
        Spacer(modifier = Modifier
            .weight(1.0f))
        Button(
            onClick = {
                model.changeUniqueName(model.getUniqueName().value, rootModel)
            },
            modifier = Modifier
                .fillMaxWidth()
                .height(50.dp),
        ) {
            Text("Сохранить")
        }
        Spacer(modifier = Modifier.height(40.dp))
    }
}

