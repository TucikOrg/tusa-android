package com.artem.tusaandroid.app.avatar

import android.graphics.Bitmap
import androidx.compose.runtime.MutableState

data class AvatarUI(
    val id: Long,
    val bitmap: MutableState<Bitmap?>,
    val byteArray: ByteArray,
    val isNetwork: Boolean
)