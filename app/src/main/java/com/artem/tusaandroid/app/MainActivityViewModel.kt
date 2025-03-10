package com.artem.tusaandroid.app

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.avatar.AvatarUI
import com.artem.tusaandroid.app.image.ImageState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.systemui.SystemUIState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.room.FriendRequestDao
import com.artem.tusaandroid.socket.EventListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import javax.inject.Inject

@HiltViewModel
class MainActivityViewModel @Inject constructor(
    val authenticationState: AuthenticationState,
    val profileState: ProfileState,
    val systemUIState: SystemUIState,
    val lastLocationState: LastLocationState,
    val imageState: ImageState,
    val avatarState: AvatarState,
    val meAvatarState: MeAvatarState,
    val locationsState: LocationsState,
    val friendsRequestsDao: FriendRequestDao,
): ViewModel() {



    fun initStateListeners() {
        imageState.initListener(viewModelScope)
        avatarState.initListeners(viewModelScope)

        avatarState.avatarEventBus.addListener(object: EventListener<AvatarUI> {
            override fun onEvent(event: AvatarUI) {
                if (event.isNetwork) {
                    if (event.id == profileState.getUserId()) {
                        // моя аватарка
                        meAvatarState.updateMeMarkerInRender()
                        return
                    }

                    // если аватарка пришла то обновляем ее в маркере если есть
                    locationsState.updateAvatarIsRequired(event.id)
                }
            }
        })
    }
}