package com.artem.tusaandroid.app.avatar

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.MeAvatarState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class LoadAvatarViewModel @Inject constructor(
    private val meAvatarState: MeAvatarState,
    private val avatarState: AvatarState
): ViewModel() {
    fun retrieveMyAvatarAndUpdateMarker() {
        avatarState.retrieveAvatar(meAvatarState.getMeId(), viewModelScope) {
            meAvatarState.updateMeMarkerInRender()
        }
    }
}