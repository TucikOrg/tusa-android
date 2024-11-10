package com.artem.tusaandroid.app

import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class AppLaunchActionsViewModel @Inject constructor(
    private val lastLocationState: LastLocationState?,
    private val socketListener: SocketListener?,
    private val profileState: ProfileState?
): ViewModel() {
    fun getLocationForegroundServiceStarted() = lastLocationState?.getLocationForegroundServiceStarted()
    fun setLocationForegroundServiceStarted(state: Boolean) = lastLocationState?.saveLocationForegroundServiceStarted(state)
    fun connectToSocket() {
        if (profileState?.getIsAuthenticated() == true) {
            profileState.getUserId().let {
                socketListener?.connect(it)
            }
        }
    }
}