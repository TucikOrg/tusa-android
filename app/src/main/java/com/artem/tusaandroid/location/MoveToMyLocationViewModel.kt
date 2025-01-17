package com.artem.tusaandroid.location

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.CameraState
import com.artem.tusaandroid.NativeLibrary
import com.artem.tusaandroid.app.systemui.SystemUIState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class MoveToMyLocationViewModel @Inject constructor(
    private val lastLocationState: LastLocationState?,
    private val cameraState: CameraState?
) : ViewModel() {
    fun moveToMe() {
        if (lastLocationState == null) return
        if (lastLocationState.getLastLocationExists().value == false) return

        val latitude = lastLocationState.getLastLatitude()
        val longitude = lastLocationState.getLastLongitude()
        cameraState?.moveTo(latitude, longitude, 17.0f)
    }
}