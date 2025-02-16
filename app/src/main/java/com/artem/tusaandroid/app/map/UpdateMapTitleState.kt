package com.artem.tusaandroid.app.map


class UpdateMapTitleState {

    var onUpdateMapTitleState: (() -> Unit)? = null

    fun updateMapTitle() {
        onUpdateMapTitleState?.invoke()
    }
}