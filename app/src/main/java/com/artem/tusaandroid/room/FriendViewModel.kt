package com.artem.tusaandroid.room

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.MutableStateFlow
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.launch
import javax.inject.Inject

@HiltViewModel
class FriendViewModel @Inject constructor(private val repository: FriendRepository): ViewModel() {
    private val _allFriends = MutableStateFlow<List<FriendEntity>>(emptyList())
    val allFriends: StateFlow<List<FriendEntity>> = _allFriends

    init {
        viewModelScope.launch {
            repository.allFriends.collect { friends ->
                _allFriends.value = friends
            }
        }
    }

    suspend fun insert(friend: FriendEntity) {
        repository.insert(friend)
    }

    suspend fun deleteByInstallAppId(installAppId: String) {
        repository.deleteByInstallAppId(installAppId)
    }
}