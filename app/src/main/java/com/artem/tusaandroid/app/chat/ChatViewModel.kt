package com.artem.tusaandroid.app.chat

import com.artem.tusaandroid.R
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
import android.media.ExifInterface
import android.net.Uri
import android.util.Log
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.produceState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.lifecycle.LifecycleOwner
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import androidx.work.Data
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.WorkInfo
import androidx.work.WorkManager
import coil.compose.AsyncImage
import com.artem.tusaandroid.app.AlineTwoLongsIds
import com.artem.tusaandroid.app.IsOnlineState
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.beauty.ShimmerBox
import com.artem.tusaandroid.app.image.ImageAttached
import com.artem.tusaandroid.app.image.ImageState
import com.artem.tusaandroid.app.image.ImageUploadWorker
import com.artem.tusaandroid.app.image.TempIdToUriDao
import com.artem.tusaandroid.app.image.TempIdToUriEntity
import com.artem.tusaandroid.app.profile.JwtGlobal
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.SendMessage
import com.artem.tusaandroid.dto.messenger.WritingMessage
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusEntity
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.room.messenger.UploadingImageStatus
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import java.io.ByteArrayOutputStream
import java.io.File
import java.io.FileOutputStream
import java.time.LocalDateTime
import java.time.ZoneOffset
import java.util.UUID
import javax.inject.Inject
import kotlin.collections.MutableMap

@HiltViewModel
open class ChatViewModel @Inject constructor(
    val chatsState: ChatState,
    val friendsState: FriendsState,
    val socketListener: SocketListener,
    val friendDao: FriendDao,
    val profileState: ProfileState,
    val messagesDao: MessageDao,
    val isOnlineState: IsOnlineState,
    val customTucikEndpoints: CustomTucikEndpoints,
    val tempIdToUriDao: TempIdToUriDao,
    val imageUploadingStatusDao: ImageUploadingStatusDao
): ViewModel() {
    var page = 1
    private var messagesStatesMap: MutableMap<Pair<Long, Long>, StateFlow<List<MessageResponse>>> = mutableMapOf()
    private var imagesStatusesStatesFlow: StateFlow<List<ImageUploadingStatusEntity>>? = null
    private var writingMessagesMap: MutableMap<Long, MutableState<String>> = mutableMapOf()
    private var updateWritingMessagesTime: MutableMap<Long, LocalDateTime> = mutableMapOf()
    private var chatAttaches: MutableMap<Long, MutableState<List<ImageAttached>>> = mutableMapOf()


    fun getIsFriendOnline(): MutableState<Boolean> {
        return isOnlineState.isUserOnline(getWithUserId())
    }

    fun getCurrentWritingMessage(): MutableState<String> {
        return getWritingMessage(getWithUserId())
    }

    fun getAttaches(): MutableState<List<ImageAttached>> {
        val withUser = getWithUserId()
        if (chatAttaches[withUser] == null) {
            chatAttaches[withUser] = mutableStateOf(listOf())
        }
        return chatAttaches[withUser]!!
    }

    fun getWritingMessage(fromUserId: Long): MutableState<String> {
        var writingMessage = writingMessagesMap[fromUserId]
        if (writingMessage == null) {
            writingMessage = mutableStateOf("")
            writingMessagesMap[fromUserId] = writingMessage
        }
        return writingMessage
    }

    fun checkWritingMessagesLife() {
        val now = LocalDateTime.now(ZoneOffset.UTC)
        updateWritingMessagesTime.forEach { (userId, time) ->
            if (now.minusSeconds(5) > time) {
                writingMessagesMap[userId]!!.value = ""
            }
        }
    }

    fun imageUriSelected(uri: Uri) {
        val withUserId = getWithUserId()
        val fileId = UUID.randomUUID().toString()
        var imageAttached = ImageAttached(uri = uri, fileLocalId = fileId)
        if (chatAttaches[withUserId] == null) {
            chatAttaches[withUserId] = mutableStateOf(listOf(imageAttached))
        } else {
            chatAttaches[withUserId]!!.value = chatAttaches[withUserId]!!.value + imageAttached
        }

        viewModelScope.launch {
            tempIdToUriDao.insert(TempIdToUriEntity(imageAttached.fileLocalId, imageAttached.uri.toString()))
        }
    }

    init {
        socketListener.getReceiveMessage().getReceiveMessenger().writingMessageBus.addListener(object : EventListener<WritingMessage> {
            override fun onEvent(data: WritingMessage) {
                if (data.toUserId != profileState.getUserId()) {
                    throw Exception("Writing message to another user")
                }
                if (writingMessagesMap[data.fromUserId] == null) {
                    writingMessagesMap[data.fromUserId] = mutableStateOf("")
                }
                updateWritingMessagesTime[data.fromUserId] = LocalDateTime.now(ZoneOffset.UTC)
                writingMessagesMap[data.fromUserId]!!.value = data.message
            }
        })

    }

    fun getMessages(): StateFlow<List<MessageResponse>> {
        val chat = chatsState.chat.value!!
        val ids = AlineTwoLongsIds.aline(chat.firstUserId, chat.secondUserId)
        val key = Pair(ids.first, ids.second)
        var messages = messagesStatesMap[key]
        if (messages == null) {
            messages = createMessagesFlow()
            messagesStatesMap[key] = messages
        }
        return messages
    }

    private fun createMessagesFlow(): StateFlow<List<MessageResponse>> {
        val chat = chatsState.chat.value!!
        val ids = AlineTwoLongsIds.aline(chat.firstUserId, chat.secondUserId)
        return messagesDao.getAllFlow(ids.first, ids.second).stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )
    }

    val chatFriend: MutableState<FriendDto?> = mutableStateOf(null)

    fun getModalOpened() = chatsState.modalOpened
    fun getChat() = chatsState.chat

    fun closeChat() {
        chatsState.closeChat()
    }

    fun getChatFriend() {
        val meId = profileState.getUserId()
        val chat = getChat().value!!
        val chatOpponentId = if (chat.firstUserId == meId) chat.secondUserId else chat.firstUserId
        viewModelScope.launch {
            chatFriend.value = friendDao.findById(chatOpponentId)
        }
    }

    fun sendMessage(message: String, context: Context, lifecycleOwner: LifecycleOwner) {
        val currentChat = chatsState.chat.value!!
        val toId = getWithUserId()

        val attachedTempIds = getAttaches().value.map { it.fileLocalId }
        val sendMessage = SendMessage(
            toId = toId,
            message = message,
            payload = attachedTempIds,
            temporaryId = UUID.randomUUID().toString()
        )

        // временно сохраняем сообщение в базу данных
        // оно будет показываться в чате как сообщение для отправки
        viewModelScope.launch {
            messagesDao.insert(MessageResponse(
                id = null,
                temporaryId = sendMessage.temporaryId,
                firstUserId = currentChat.firstUserId,
                secondUserId = currentChat.secondUserId,
                senderId = profileState.getUserId(),
                message = message,
                updateTime = LocalDateTime.now(ZoneOffset.UTC).toEpochSecond(ZoneOffset.UTC),
                payload = attachedTempIds.joinToString(separator = ","),
                deleted = false
            ))
        }

        val attaches = getAttaches().value
        if (attaches.isEmpty()) {
            // ели нету приложений то сразу отправялем сообщение
            socketListener.getSendMessage()?.sendChatMessage(sendMessage)
        }

        // все выбранные картинки грузим на сервер
        for (imageAttached in attaches) {

            var uri = imageAttached.uri
            val fileId = imageAttached.fileLocalId
            uploadImageToServer(
                uri = uri,
                fileId = fileId,
                sendMessage = sendMessage,
                context = context,
                lifecycleOwner = lifecycleOwner
            )
        }

        // очистит все прикрепленные картинки
        chatAttaches[getWithUserId()]!!.value = listOf()
    }

    fun uploadImageToServer(
        uri: Uri,
        fileId: String,
        sendMessage: SendMessage,
        context: Context,
        lifecycleOwner: LifecycleOwner
    ) {
        // учитываем exif данные
        // выравниваем изображение по повороту
        var matrix: Matrix?
        var bitmap: Bitmap?
        context.contentResolver.openInputStream(uri).use { inputStream ->
            val exif = ExifInterface(inputStream!!)
            val orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL)
            val rotationDegrees = when (orientation) {
                ExifInterface.ORIENTATION_ROTATE_90 -> 90
                ExifInterface.ORIENTATION_ROTATE_180 -> 180
                ExifInterface.ORIENTATION_ROTATE_270 -> 270
                else -> 0
            }
            matrix = Matrix().apply { postRotate(rotationDegrees.toFloat()) }
        }
        context.contentResolver.openInputStream(uri).use { inputStream ->
            val originalBitmap = BitmapFactory.decodeStream(inputStream)
            bitmap = Bitmap.createBitmap(originalBitmap, 0, 0, originalBitmap.width, originalBitmap.height, matrix, true)
        }

        // сжимаем файл и сохранем во временный файлы
        var filePath = ""
        ByteArrayOutputStream().use {
            bitmap!!.compress(Bitmap.CompressFormat.JPEG, 70, it)
            val bytesArray = it.toByteArray()

            val tempFile = File.createTempFile(fileId, null, context.cacheDir)
            FileOutputStream(tempFile).use { fos ->
                fos.write(bytesArray)
            }
            filePath = tempFile.path
        }

        // запускаем загрузку файлов на сервер
        val uploadWorkRequest = OneTimeWorkRequestBuilder<ImageUploadWorker>()
            .setInputData(
                Data.Builder()
                    .putString("file_path", filePath)
                    .putString("jwt", JwtGlobal.jwt)
                    .putString("url", customTucikEndpoints.makeImageUpload())
                    .putString("file_id", fileId)
                    .putString("message_temp_id", sendMessage.temporaryId)
                    .build()
            )
            .build()
        WorkManager.getInstance(context).enqueue(uploadWorkRequest)

        // слушаем когда загрузка картинки будет завершена
        WorkManager.getInstance(context).getWorkInfoByIdLiveData(uploadWorkRequest.id)
            .observe(lifecycleOwner) { workInfo ->
                viewModelScope.launch {
                    if (workInfo != null && workInfo.state.isFinished) {
                        val uploaded = workInfo.outputData.getBoolean("uploaded", false)
                        if (uploaded) {
                            val messageTempId = workInfo.outputData.getString("message_temp_id")!!
                            val message = messagesDao.findByTempId(messageTempId)!!
                            val payload = message.getClearedPayload()
                            val statuses = imageUploadingStatusDao.findByTempIdIn(payload)
                            val isAllUploaded = statuses.all { it.status == UploadingImageStatus.UPLOADED.ordinal }
                            if (isAllUploaded) {
                                socketListener.getSendMessage()?.sendChatMessage(sendMessage)
                            }
                        }
                    }
                }
            }
    }

    fun loadMoreItems() {
        val withUserId = getWithUserId()

        val page = getMessages().value.size / MessagesConsts.batchSize
        socketListener.getSendMessage()?.messages(
            withUserId = withUserId,
            page = page,
            size = MessagesConsts.batchSize
        )
    }

    fun writingMessage(message: String) {
        val withUserId = getWithUserId()
        socketListener.getSendMessage()?.writingMessage(withUserId, message)
    }

    fun getWithUserId(): Long {
        val currentChat = chatsState.chat.value!!
        val withUserId = if (currentChat.firstUserId == profileState.getUserId())
            currentChat.secondUserId else currentChat.firstUserId
        return withUserId
    }

    fun removeAttach(uri: Uri) {
        val withUserId = getWithUserId()
        chatAttaches[withUserId]!!.value = chatAttaches[withUserId]!!.value.filter { it.uri != uri }
    }
}