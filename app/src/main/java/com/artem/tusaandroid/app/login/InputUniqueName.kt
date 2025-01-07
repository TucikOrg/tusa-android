package com.artem.tusaandroid.app.login

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@Composable
@Preview
fun InputUniqueNamePreview() {
    InputUniqueName(
        model = InputUniqueNameViewModel(null),
    )
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun InputUniqueName(model: InputUniqueNameViewModel) {
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    if (!model.showModal.value) {
        return
    }

    ModalBottomSheet(
        shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
        sheetState = sheetState,
        onDismissRequest = {
            model.showModal.value = false
        }
    ) {
        Column(
            modifier = Modifier
                .fillMaxSize()
                .padding(horizontal = 10.dp)
                .padding(bottom = 10.dp),
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
                leadingIcon = {
                    Text(
                        text = "@",
                        style = MaterialTheme.typography.headlineSmall,
                        fontWeight = FontWeight.Bold
                    )
                },
                keyboardActions = KeyboardActions(
                    onDone = {
                        model.changeUniqueName(model.getUniqueName().value)
                    }
                ),
                singleLine = true,
                enabled = true,
                textStyle = MaterialTheme.typography.bodyLarge,
                shape = RoundedCornerShape(10.dp),
                modifier = Modifier
                    .fillMaxWidth()
                    .padding(horizontal = 10.dp, vertical = 5.dp)
            )
            Spacer(modifier = Modifier
                .weight(1.0f))
            Button(
                onClick = {
                    model.changeUniqueName(model.getUniqueName().value)
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
}

