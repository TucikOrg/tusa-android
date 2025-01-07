package com.artem.tusaandroid.app.dialog

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf

class AppDialogState {
    var opened: MutableState<Boolean> = mutableStateOf(false)
    var message: MutableState<String> = mutableStateOf("")
    var title: MutableState<String> = mutableStateOf("")

    fun open(title: String, message: String, ) {
        this.title.value = title
        this.message.value = message
        opened.value = true
    }
}