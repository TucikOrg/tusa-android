package com.artem.tusaandroid.app.action.friends

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class FriendRowInSearchViewModel @Inject constructor(
    private val friendsState: FriendsState?
): ViewModel() {
    fun addFriend(friend: FriendDto) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                friendsState?.addFriend(friend)
            }
        }
    }

    fun checkAlreadyFriend(id: Long): Boolean {
        return friendsState?.friends?.value?.any {it.id == id} == true
    }
}