package com.artem.tusaandroid.app.chat

import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AlineTwoLongsIds
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.socket.SocketListener
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import java.time.LocalDateTime
import java.time.ZoneOffset
import java.util.UUID
import kotlin.Long

class ChatState(
    private val profileState: ProfileState,
    private val chatDao: ChatDao,
    private val friendDao: FriendDao,
    private val socketListener: SocketListener,
    private val messagesDao: MessageDao
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

                // чат временный
                // только что открыли его с пользователем
                // настоящий чат создастся при отправке сообщения
                useChat = ChatResponse(
                    id = null,
                    firstUserId = ids.first,
                    secondUserId = ids.second,
                    firsUserName = firstUserName,
                    secondUserName = secondUserName,
                    firstUserUniqueName = firstUserUniqueName,
                    secondUserUniqueName = secondUniqueName,
                    updateTime = 0,
                    deleted = false
                )
            }

            openChat(useChat)
        }
    }

    fun resendMessage(viewModelScope: CoroutineScope, message: MessageResponse) {
        val toId = if (message.firstUserId == profileState.getUserId()) message.secondUserId else message.firstUserId
        val sendMessage = SendMessage(
            toId = toId,
            message = message.message,
            payload = listOf(),
            temporaryId = message.temporaryId
        )
        socketListener.getSendMessage()?.sendChatMessage(sendMessage)
    }

    fun openChat(chat: ChatResponse) {
        this.chat.value = chat
        modalOpened.value = true
    }
}


