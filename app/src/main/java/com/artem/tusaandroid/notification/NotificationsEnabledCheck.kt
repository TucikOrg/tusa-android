package com.artem.tusaandroid.notification

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.platform.LocalContext

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun NotificationsEnabledCheck(model: NotificationsEnabledCheckViewModel) {
    val context = LocalContext.current
    val showDialog = model.getShowDialog()
    if (showDialog.value) {
        AlertDialog(
            title = {
                Text("Уведомления")
            },
            text = {
                Text("Факт работы приложения в фоновом режиме будет отображаться там. Разрешите для удобства.")
            },
            dismissButton = {
                TextButton(
                    onClick = {
                        showDialog.value = false
                    }
                ) {
                    Text("Закрыть")
                }
            },
            confirmButton = {
                TextButton(
                    onClick = {
                        model.goToSettings(context)
                        showDialog.value = false
                    }
                ) {
                    Text("Разрешить")
                }
            },
            onDismissRequest = {
                showDialog.value = false
            },
        )
    }

    LaunchedEffect(Unit) {
        model.check(context)
    }
}