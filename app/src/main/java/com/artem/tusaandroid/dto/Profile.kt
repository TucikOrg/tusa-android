package com.artem.tusaandroid.dto

import androidx.compose.runtime.MutableState

class Profile(
    val name: MutableState<String>,
    val uniqueName: MutableState<String>,
    val gmail: MutableState<String>,
)