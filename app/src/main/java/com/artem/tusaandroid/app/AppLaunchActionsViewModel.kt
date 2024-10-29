package com.artem.tusaandroid.app

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.location.LastLocationState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class AppLaunchActionsViewModel @Inject constructor(
    private val lastLocationState: LastLocationState?,
    private val startStopApp: StartStopApp?
): ViewModel() {
    fun getLocationForegroundServiceStarted() = lastLocationState?.getLocationForegroundServiceStarted()
    fun setLocationForegroundServiceStarted(state: Boolean) = lastLocationState?.saveLocationForegroundServiceStarted(state)
    fun onLogin() {
        startStopApp?.onLogin()
    }
}