package com.artem.tusaandroid.app.image

import android.net.Uri

data class ImageAttached(
    val uri: Uri,
    var fileLocalId: String
)