package com.artem.tusaandroid.app.dialog

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable

@Composable
fun AppDialog(model: AppDialogViewModel) {
    val state = model.getAppDialogState()
    if (state.opened.value) {
        AlertDialog(
            title = {
                Text(state.title.value)
            },
            text = {
                Text(state.message.value)
            },
            onDismissRequest = {
                state.opened.value = false
            },
            dismissButton = {},
            confirmButton = {},
        )
    }
}