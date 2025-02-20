package com.artem.tusaandroid.app.action.friends

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.avatar.AvatarState
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class FriendAvatarViewModel @Inject constructor(
    private val avatarsState: AvatarState?
): ViewModel() {
    fun getState(userId: Long) = avatarsState?.getAvatarBitmap(userId)

    fun retrieveAvatar(userId: Long) {
        avatarsState?.retrieveAvatar(userId, viewModelScope)
    }
}
