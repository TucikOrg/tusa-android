package com.artem.tusaandroid.app.systemui

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf

class SystemUIState {
    private var isLight: MutableState<Boolean> = mutableStateOf(false)

    fun setLight(isLight: Boolean) {
        IsLightGlobal.isLight = isLight
        this.isLight.value = isLight
    }

    fun getIsLight(): MutableState<Boolean> {
        return isLight
    }
}