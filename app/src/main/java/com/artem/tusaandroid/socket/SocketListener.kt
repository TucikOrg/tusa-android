package com.artem.tusaandroid.socket

import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import okio.ByteString
import java.util.concurrent.Executors

class SocketListener(
    private val socketUrl: String,
    private val client: OkHttpClient,
    private val socketConnectionState: SocketConnectionState,
) : WebSocketListener() {
    private val reconnectExecutor = Executors.newSingleThreadExecutor()
    private lateinit var webSocket: WebSocket
    private var sendMessage: SendMessage? = null
    private var receiveMessage: ReceiveMessage = ReceiveMessage()
    private var meUserId: Long = 0

    fun getReceiveMessage(): ReceiveMessage {
        return receiveMessage
    }

    fun getSendMessage(): SendMessage? {
        return sendMessage
    }

    fun disconnect() {
        val result = webSocket.close(1000, "Goodbye")
    }

    fun connect(meId: Long?) {
        val request = Request.Builder()
            .url(socketUrl)
            .build()
        webSocket = client.newWebSocket(request, this)
        sendMessage = SendMessage(webSocket)

        if (meId != null) {
            meUserId = meId
            sendMessage?.loadAvatar(meId)
            sendMessage?.loadFriendsAndRequests()
            sendMessage?.loadFriendsAvatars()
        }
    }

    override fun onOpen(webSocket: WebSocket, response: okhttp3.Response) {
        super.onOpen(webSocket, response)
        socketConnectionState.state.value = SocketConnectionStates.OPEN
    }

    override fun onMessage(webSocket: WebSocket, bytes: ByteString) {
        super.onMessage(webSocket, bytes)
        try {
            receiveMessage.receiveBytesMessage(bytes)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun onClosed(webSocket: WebSocket, code: Int, reason: String) {
        super.onClosed(webSocket, code, reason)
        socketConnectionState.closed()
    }

    override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
        super.onClosing(webSocket, code, reason)
        println("Closing: $code / $reason")
    }


    override fun onFailure(webSocket: WebSocket, t: Throwable, response: okhttp3.Response?) {
        super.onFailure(webSocket, t, response)
        socketConnectionState.failed()
        println("Error: ${t.message}")

        val connectionLoosed =
                t is java.net.SocketException ||
                t is java.net.SocketTimeoutException ||
                t is java.io.EOFException

        if (connectionLoosed) {
            // Пробуем восстановить соединение через время
            socketConnectionState.waitToReconnect()
            reconnectExecutor.submit {
                Thread.sleep(2000)
                // после попытки если не получиться то опять бросит onFailure()
                // и так он будет в итоге каждые 2 секунды пытаться переподключиться
                connect(meUserId)
            }
        }
    }
}