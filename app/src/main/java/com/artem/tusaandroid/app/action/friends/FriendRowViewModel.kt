package com.artem.tusaandroid.app.action.friends

import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.R
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import javax.inject.Inject

@HiltViewModel
open class FriendRowViewModel @Inject constructor(
    private val friendsState: FriendsState?
): ViewModel() {
    val requestSent = mutableStateOf(false)

    fun removeFriend(id: Long) {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                friendsState?.removeFriend(id)
            }
        }
    }
}
