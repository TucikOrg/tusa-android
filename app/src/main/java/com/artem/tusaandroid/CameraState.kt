package com.artem.tusaandroid

import com.artem.tusaandroid.app.systemui.SystemUIState

class CameraState(
    private val systemUIState: SystemUIState?
) {
    fun moveTo(latitude: Float, longitude: Float, zoom: Float) {
        NativeLibrary.setCameraPos(latitude, longitude, zoom)
        systemUIState?.setLight(zoom > 3.0f)
    }
}