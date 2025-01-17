package com.artem.tusaandroid.app.action.friends

import androidx.lifecycle.ViewModel
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class FriendRowViewModel @Inject constructor(
    private val friendsState: FriendsState
): ViewModel() {
    fun removeFriend(id: Long) {
        friendsState.removeFriend(id)
    }
}
