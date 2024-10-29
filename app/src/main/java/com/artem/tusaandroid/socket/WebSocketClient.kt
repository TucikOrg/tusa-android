package com.artem.tusaandroid.socket

import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.WebSocket
import okhttp3.WebSocketListener
import okio.ByteString
import okio.ByteString.Companion.toByteString

class WebSocketClient(
    private val client: OkHttpClient,
    private val socketUrl: String,
    private val receive: ReceiveMessage
) : WebSocketListener() {
    private lateinit var webSocket: WebSocket

    fun connect() {
        val request = Request.Builder()
            .url(socketUrl)
            .build()

        webSocket = client.newWebSocket(request, this)
    }

    fun sendMessage(message: ByteArray) {
        webSocket.send(message.toByteString())
    }

    override fun onOpen(webSocket: WebSocket, response: okhttp3.Response) {
        super.onOpen(webSocket, response)
    }

    override fun onMessage(webSocket: WebSocket, bytes: ByteString) {
        super.onMessage(webSocket, bytes)
        try {
            receive.receiveBytesMessage(bytes)
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    override fun onClosing(webSocket: WebSocket, code: Int, reason: String) {
        super.onClosing(webSocket, code, reason)
        webSocket.close(1000, null)
        println("Closing: $code / $reason")
    }

    override fun onFailure(webSocket: WebSocket, t: Throwable, response: okhttp3.Response?) {
        super.onFailure(webSocket, t, response)
        println("Error: ${t.message}")
    }

    fun disconnect() {
        webSocket.close(1000, "Goodbye")
    }
}