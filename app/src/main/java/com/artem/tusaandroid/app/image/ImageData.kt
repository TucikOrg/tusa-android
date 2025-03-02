package com.artem.tusaandroid.app.image

import android.graphics.Bitmap
import android.net.Uri
import androidx.compose.runtime.MutableState

data class ImageData(
    val uri: Uri?,
    val bitmap: MutableState<Bitmap?>
)