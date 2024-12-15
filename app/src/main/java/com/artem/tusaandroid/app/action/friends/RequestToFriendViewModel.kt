package com.artem.tusaandroid.app.action.friends

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.R
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class RequestToFriendViewModel @Inject constructor(
    private val friendsState: FriendsState?
): ViewModel() {


    fun acceptRequest(id: Long) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                friendsState?.acceptRequest(id)
            }
        }
    }

    fun removeRequestToFriend(id: Long) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                friendsState?.removeRequestToFriend(id)
            }
        }
    }
}