package com.artem.tusaandroid.app.admin

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.app.action.AdminFabViewModel
import com.artem.tusaandroid.app.systemui.IsLightGlobal

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CreateUserOption(model: AdminFabViewModel) {
    if (model.showCreateUser) {
        ModalBottomSheet(
            shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
            onDismissRequest = { model.showCreateUser = false },
            sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true, ),
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = !IsLightGlobal.isLight
            ),
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing)
        ) {
            var uniqueName by remember {
                mutableStateOf("")
            }
            var gmail by remember {
                mutableStateOf("")
            }

            Column(
                modifier = Modifier.padding(10.dp)
            ) {
                val keyboard = LocalSoftwareKeyboardController.current
                OutlinedTextField(
                    value = uniqueName,
                    onValueChange = {
                        uniqueName = it
                    },
                    label = { Text("Уникальное имя") },
                    keyboardActions = KeyboardActions(
                        onDone = {
                            keyboard?.hide()
                        }
                    ),
                    singleLine = true,
                    enabled = true,
                    textStyle = MaterialTheme.typography.bodyMedium.copy(textAlign = TextAlign.Start),
                    modifier = Modifier.fillMaxWidth()
                )
                Spacer(modifier = Modifier.height(10.dp))
                OutlinedTextField(
                    value = gmail,
                    onValueChange = {
                        gmail = it
                    },
                    label = { Text("Gmail") },
                    keyboardActions = KeyboardActions(
                        onDone = {
                            keyboard?.hide()
                        }
                    ),
                    singleLine = true,
                    enabled = true,
                    textStyle = MaterialTheme.typography.bodyMedium.copy(textAlign = TextAlign.Start),
                    modifier = Modifier.fillMaxWidth()
                )
                Spacer(modifier = Modifier.height(10.dp))
                ElevatedButton(
                    modifier = Modifier.fillMaxWidth(),
                    shape = RoundedCornerShape(10.dp),
                    onClick = {
                        model.createUser(uniqueName, gmail)
                        model.showCreateUser = false
                    }
                ) {
                    Text(
                        text = "Создать",
                        fontWeight = FontWeight.Bold,
                        fontSize = MaterialTheme.typography.bodyMedium.fontSize
                    )
                }
            }
        }
    }

    ElevatedButton(
        modifier = Modifier.fillMaxWidth(),
        shape = RoundedCornerShape(10.dp),
        onClick = {
            model.showCreateUser = true
        }
    ) {
        Text(
            text = "Создать пользователя",
            fontWeight = FontWeight.Bold,
            fontSize = MaterialTheme.typography.bodyMedium.fontSize
        )
    }
}