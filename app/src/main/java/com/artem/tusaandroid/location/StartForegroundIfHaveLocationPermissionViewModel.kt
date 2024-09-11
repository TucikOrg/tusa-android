package com.artem.tusaandroid.location

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.AppVariables
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class StartForegroundIfHaveLocationPermissionViewModel @Inject constructor(
    private val lastLocationState: LastLocationState?
): ViewModel() {
    fun getLocationForegroundServiceStarted() = lastLocationState?.getLocationForegroundServiceStarted()
}