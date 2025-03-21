package com.artem.tusaandroid.socket

import com.artem.tusaandroid.dto.ChatsResponse
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.ResponseMessages
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.dto.messenger.InitChatsResponse
import com.artem.tusaandroid.dto.messenger.InitMessagesResponse
import com.artem.tusaandroid.dto.messenger.WritingMessage
import kotlinx.serialization.ExperimentalSerializationApi
import kotlinx.serialization.cbor.Cbor
import kotlinx.serialization.decodeFromByteArray

class ReceiveMessenger {
    val messagesBus: EventBus<ResponseMessages> = EventBus()
    val chatsBus: EventBus<ChatsResponse> = EventBus()
    val chatsActionsBus: EventBus<List<ChatResponse>> = EventBus()
    val messagesActionsBus: EventBus<List<MessageResponse>> = EventBus()
    val refreshChats: EventBus<Unit> = EventBus()
    val refreshMessages: EventBus<Unit> = EventBus()
    val messagesInitBus: EventBus<InitMessagesResponse> = EventBus()
    val chatsInitBus: EventBus<InitChatsResponse> = EventBus()
    val writingMessageBus: EventBus<WritingMessage> = EventBus()

    @OptIn(ExperimentalSerializationApi::class)
    fun handleMessage(socketBinaryMessage: SocketBinaryMessage) {
        when (socketBinaryMessage.type) {

            "writing-message" -> {
                val writingMessage = Cbor.decodeFromByteArray<WritingMessage>(socketBinaryMessage.data)
                writingMessageBus.pushEvent(writingMessage)
            }
            "init-messages" -> {
                val initMessagesResponse = Cbor.decodeFromByteArray<InitMessagesResponse>(socketBinaryMessage.data)
                messagesInitBus.pushEvent(initMessagesResponse)
            }
            "init-chats" -> {
                val initChatsResponse = Cbor.decodeFromByteArray<InitChatsResponse>(socketBinaryMessage.data)
                chatsInitBus.pushEvent(initChatsResponse)
            }
            "chats-actions" -> {
                val chatsActions = Cbor.decodeFromByteArray<List<ChatResponse>>(socketBinaryMessage.data)
                chatsActionsBus.pushEvent(chatsActions)
            }
            "messages-actions" -> {
                val messagesActions = Cbor.decodeFromByteArray<List<MessageResponse>>(socketBinaryMessage.data)
                messagesActionsBus.pushEvent(messagesActions)
            }
            "messages" -> {
                val messages = Cbor.decodeFromByteArray<ResponseMessages>(socketBinaryMessage.data)
                messagesBus.pushEvent(messages)
            }
            "chats" -> {
                val chats = Cbor.decodeFromByteArray<ChatsResponse>(socketBinaryMessage.data)
                chatsBus.pushEvent(chats)
            }
            "refresh-chats" -> {
                refreshChats.pushEvent(Unit)
            }
            "refresh-messages" -> {
                refreshMessages.pushEvent(Unit)
            }
        }
    }
}