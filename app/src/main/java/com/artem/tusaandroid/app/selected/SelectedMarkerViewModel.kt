package com.artem.tusaandroid.app.selected

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.CameraState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.room.FriendDao
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.launch
import javax.inject.Inject


@HiltViewModel
open class SelectedMarkerViewModel @Inject constructor(
    val selectedState: SelectedState,
    val locationsState: LocationsState,
    val chatState: ChatState,
    val cameraState: CameraState,
    val friendsDao: FriendDao
): ViewModel() {
    val friend: MutableState<FriendDto?> = mutableStateOf(null)

    fun openChatWithUser(userId: Long) {
        chatState.openChatWithUser(userId, viewModelScope)
    }

    fun loadSelectedFriend(userId: Long) {
        viewModelScope.launch {
            friend.value = friendsDao.findById(userId)
        }
    }
}