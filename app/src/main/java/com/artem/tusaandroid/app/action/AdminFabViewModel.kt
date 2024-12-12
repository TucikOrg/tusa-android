package com.artem.tusaandroid.app.action

import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.setValue
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.dto.AddUserDto
import com.artem.tusaandroid.dto.AllUsersRequest
import com.artem.tusaandroid.dto.User
import com.artem.tusaandroid.dto.UsersPage
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class AdminFabViewModel @Inject constructor(
    private val socketListener: SocketListener?,
): ViewModel() {
    var showModal by mutableStateOf(false)
    var allUsersModal by mutableStateOf(false)
    var showCreateUser by mutableStateOf(false)

    var users by mutableStateOf(listOf<User>())
    var usersPageSize = 10
    var uniqueName = ""
    var usersPagesCount = 1

    init {
        socketListener?.getReceiveMessage()?.allUsers?.addListener(object : EventListener<UsersPage> {
            override fun onEvent(data: UsersPage) {
                users = users.plus(data.users)
                usersPagesCount = data.pagesCount
            }
        })
    }

    fun restart() {
        users = listOf()
        usersPagesCount = 1
        loadMoreUsers()
    }

    fun createUser(uniqueName: String, gmail: String) {
        socketListener?.getSendMessage()?.createUser(AddUserDto(uniqueName, gmail))
    }

    fun loadMoreUsers() {
        var page = users.size / usersPageSize
        if (page < usersPagesCount && users.size % usersPageSize == 0) {
            socketListener?.getSendMessage()?.allUsers(AllUsersRequest(page, usersPageSize, uniqueName))
        }
    }
}