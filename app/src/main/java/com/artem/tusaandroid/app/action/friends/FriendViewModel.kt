package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestDto
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.FriendRequestDao
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import javax.inject.Inject

@HiltViewModel
open class FriendViewModel @Inject constructor(
    private val friendsState: FriendsState,
    private val friendsDao: FriendDao,
    private val friendsRequestDao: FriendRequestDao
): ViewModel() {
    val showModal = mutableStateOf(false)

    val friends: StateFlow<List<FriendDto>> = friendsDao.getAllFlow()
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )

    val requests: StateFlow<List<FriendRequestDto>> = friendsRequestDao.getRequestsToMeFlow()
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )
}