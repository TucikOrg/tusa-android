package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.FriendRequestDao
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class FriendRowInSearchViewModel @Inject constructor(
    private val friendsState: FriendsState?,
    private val friendDao: FriendDao?,
    private val friendRequestDao: FriendRequestDao?
): ViewModel() {
    val friendRequestRowState: MutableState<FriendRequestRowState?> = mutableStateOf(null)

    fun addFriend(friend: FriendDto) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                friendsState?.addFriend(friend)
            }
        }
    }

    fun check(id: Long) {
        viewModelScope.launch {
            val friends = friendDao?.findById(id) != null
            if (friends) {
                friendRequestRowState.value = FriendRequestRowState.FRIENDS
                return@launch
            }

            val friendRequest = friendRequestDao?.findById(id)
            if (friendRequest == null) {
                friendRequestRowState.value = FriendRequestRowState.NOT_FRIENDS
                return@launch
            }

            if (friendRequest.isRequestSender) {
                friendRequestRowState.value = FriendRequestRowState.REQUEST_RECEIVED
                return@launch
            }

            friendRequestRowState.value = FriendRequestRowState.REQUEST_SENT
        }
    }
}