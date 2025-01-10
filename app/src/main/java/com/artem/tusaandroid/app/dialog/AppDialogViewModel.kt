package com.artem.tusaandroid.app.dialog

import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class AppDialogViewModel @Inject constructor(
    private val appDialogState: AppDialogState
): ViewModel() {
    fun getAppDialogState() = appDialogState
    fun close(dialog: AppDialog) {
        appDialogState.close(dialog)
    }
}