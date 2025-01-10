package com.artem.tusaandroid.app.dialog

import androidx.compose.material3.AlertDialog
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import com.artem.tusaandroid.location.GpsDisabledDialog

@Composable
fun AppDialog(model: AppDialogViewModel) {
    val state = model.getAppDialogState()
    for (dialog in state.dialogs.value) {
        if (dialog.gpsDisabledDialog != null) {
            GpsDisabledDialog(dialog.gpsDisabledDialog!!)
            continue
        }

        AlertDialog(
            title = {
                Text(dialog.title.value)
            },
            text = {
                Text(dialog.message.value)
            },
            onDismissRequest = {
                model.close(dialog)
            },
            dismissButton = {},
            confirmButton = {},
        )
    }
}