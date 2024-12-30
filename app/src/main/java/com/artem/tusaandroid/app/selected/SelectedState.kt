package com.artem.tusaandroid.app.selected

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf

class SelectedState {
    var selectedMarker: MutableState<Long>? = mutableStateOf(0)
}