package com.artem.tusaandroid.app.dialog

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.location.LocationSetupCardViewModel

data class AppDialog(
    var message: MutableState<String>,
    var title: MutableState<String>,
    var gpsDisabledDialog: LocationSetupCardViewModel? = null
)

class AppDialogState {
    val dialogs = mutableStateOf<List<AppDialog>>(emptyList())

    fun open(title: String, message: String, ) {
        dialogs.value = dialogs.value + AppDialog(
            title = mutableStateOf(title),
            message = mutableStateOf(message)
        )
    }

    fun close(dialog: AppDialog) {
        dialogs.value = dialogs.value.filter { it != dialog }
    }

    fun closeGpsDisabledAlert() {
        dialogs.value = dialogs.value.filter { it.gpsDisabledDialog == null }
    }

    fun showGpsDisabledAlert(locationSetupCardViewModel: LocationSetupCardViewModel) {
        dialogs.value = dialogs.value + AppDialog(
            title = mutableStateOf(""),
            message = mutableStateOf(""),
            gpsDisabledDialog = locationSetupCardViewModel
        )
    }
}