package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.room.FriendRoomEntity
import com.artem.tusaandroid.room.FriendRepository
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class FriendViewModel @Inject constructor(
    private val repository: FriendRepository?,
    private val friendsState: FriendsState?
): ViewModel() {
    val showModal = mutableStateOf(false)

    fun getFriends(): MutableState<List<FriendDto>> {
        return friendsState?.friends ?: mutableStateOf(listOf())
    }

    fun getRequests(): MutableState<List<FriendRequestDto>> {
        return friendsState?.toMeRequests ?: mutableStateOf(listOf())
    }

    suspend fun insert(friend: FriendRoomEntity) {
        repository?.insert(friend)
    }
}