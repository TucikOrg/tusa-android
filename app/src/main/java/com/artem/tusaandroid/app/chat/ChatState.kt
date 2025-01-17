package com.artem.tusaandroid.app.chat

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import com.artem.tusaandroid.app.AlineTwoLongsIds
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.ChatDao
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import kotlin.Long

class ChatState(
    private val profileState: ProfileState,
    private val chatDao: ChatDao,
    private val friendDao: FriendDao
) {
    var chat: MutableState<ChatResponse?> = mutableStateOf(null)
    val modalOpened = mutableStateOf(false)

    fun getUserId(): Long {
        return profileState.getUserId()
    }

    fun closeChat() {
        modalOpened.value = false
        chat.value = null
    }

    fun openChatWithUser(userId: Long, viewModelScope: CoroutineScope) {
        val myId = getUserId()
        val ids = AlineTwoLongsIds.aline(myId, userId)
        val meId = profileState.getUserId()

        viewModelScope.launch {

            var useChat = chatDao.findChatByUsers(ids.first, ids.second)
            if (useChat == null) {

                // для опредления чата которого еще нет на сервере
                // предварительное открытие чата с пустым id
                val chatWithFriend = friendDao.findById(userId)!!

                var firstUserName = chatWithFriend.name
                var firstUserUniqueName = chatWithFriend.uniqueName

                var secondUserName = profileState.getName().value
                var secondUniqueName = profileState.getUniqueName().value

                if (meId == ids.first) {
                    firstUserName = profileState.getName().value
                    firstUserUniqueName = profileState.getUniqueName().value

                    secondUserName = chatWithFriend.name
                    secondUniqueName = chatWithFriend.uniqueName
                }

                useChat = ChatResponse(
                    id = null,
                    firstUserId = ids.first,
                    secondUserId = ids.second,
                    firsUserName = firstUserName,
                    secondUserName = secondUserName,
                    firstUserUniqueName = firstUserUniqueName,
                    secondUserUniqueName = secondUniqueName
                )
            }

            openChat(useChat)
        }
    }

    fun openChat(chat: ChatResponse) {
        this.chat.value = chat
        modalOpened.value = true
    }
}


