package com.artem.tusaandroid.app.state

import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.app.chat.MessagesConsts
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.toast.ToastsState
import com.artem.tusaandroid.dto.AvatarForCheck
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.dto.FriendRequestDto
import com.artem.tusaandroid.dto.FriendsInitializationState
import com.artem.tusaandroid.dto.FriendsRequestsInitializationState
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.ResponseMessages
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.artem.tusaandroid.dto.messenger.InitChatsResponse
import com.artem.tusaandroid.dto.messenger.InitMessagesResponse
import com.artem.tusaandroid.dto.messenger.InitMessenger
import com.artem.tusaandroid.firebase.FirebaseState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.room.AvatarDao
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.FriendRequestDao
import com.artem.tusaandroid.room.StateTimePoint
import com.artem.tusaandroid.room.StateTypes
import com.artem.tusaandroid.room.StatesTimePointDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.ImageUploadingStatusDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.room.messenger.UnreadMessages
import com.artem.tusaandroid.room.messenger.UnreadMessagesDao
import com.artem.tusaandroid.room.messenger.UploadingImageStatus
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketBinaryMessage
import com.artem.tusaandroid.socket.SocketConnect
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketConnectionStates
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import kotlinx.serialization.ExperimentalSerializationApi
import javax.inject.Inject

@HiltViewModel
class RefreshStateListenersViewModel @Inject constructor(
    private val socketListener: SocketListener,
    private val statesTimePointsDao: StatesTimePointDao,
    private val friendDao: FriendDao,
    private val friendRequestDao: FriendRequestDao,
    private val chatDao: ChatDao,
    private val messageDao: MessageDao,
    private val avatarState: AvatarState,
    private val socketConnectionState: SocketConnectionState,
    private val locationState: LocationsState,
    private val authenticationState: AuthenticationState,
    private val socketConnect: SocketConnect,
    private val chatState: ChatState,
    private val firebaseState: FirebaseState,
    private val imageUploadingStatusDao: ImageUploadingStatusDao,
    private val toastsState: ToastsState,
    private val avatarDao: AvatarDao,
    private val unreadMessagesDao: UnreadMessagesDao,
    private val profileState: ProfileState,
    @ApplicationContext private val applicationContext: Context
): ViewModel() {

    private fun refreshAvatars() {
        val avatars = avatarDao.getAllWithoutBytes()
        val checkAvatars = avatars.map { AvatarForCheck(
            ownerId = it.id,
            updatingTime = it.updatingTime
        ) }
        socketListener.getSendMessage()?.refreshAvatars(checkAvatars)
    }

    // Друзья
    private val refreshFriendsListener = object : EventListener<Unit> {
        override fun onEvent(event: Unit) {
            viewModelScope.launch {
                val state = statesTimePointsDao.getStateTimePointByType(StateTypes.FRIENDS.ordinal)
                if (state != null) {
                    // если нету состояния то его инициализирует другой компонент
                    socketListener.getSendMessage()?.friendsActions(state)
                } else {
                    // пробуем инициализировать состояние друзей
                    socketListener.getSendMessage()?.loadFriends()
                }
            }
        }
    }
    private val friendMutes = Mutex()
    private val actionsFriendsListener = object : EventListener<List<FriendDto>> {
        override fun onEvent(event: List<FriendDto>) {
            viewModelScope.launch {
                friendMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.FRIENDS.ordinal)!!
                    var friends = event.filter { it.updateTime > state.timePoint }.sortedBy { it.updateTime }
                    for (friend in friends) {
                        if (friend.deleted) {
                            friendDao.deleteById(friend.id)
                            locationState.removeLocation(friend.id)
                        } else {
                            friendDao.insert(friend)
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.updateTime }
                        statesTimePointsDao.insert(StateTimePoint(StateTypes.FRIENDS.ordinal, maxTimePoint))
                    }
                }
            }
        }
    }
    private val initFriendsListener = object: EventListener<FriendsInitializationState> {
        override fun onEvent(event: FriendsInitializationState) {
            viewModelScope.launch {
                friendDao.clearAll()
                friendDao.insertAll(event.friends)
                statesTimePointsDao.insert(StateTimePoint(StateTypes.FRIENDS.ordinal, event.timePoint))
            }
            socketListener.getReceiveMessage().friendsBus.removeListener(this)
        }
    }

    // запросы в друзья
    private val refreshFriendsRequestsListener = object : EventListener<Unit> {
        override fun onEvent(event: Unit) {
            viewModelScope.launch {
                val state = statesTimePointsDao.getStateTimePointByType(StateTypes.FRIENDS_REQUESTS.ordinal)
                if (state != null) {
                    // Если нету состояния то его инициализирует другой компонент
                    socketListener.getSendMessage()?.friendsRequestsActions(state)
                } else {
                    // пробуем инициализировать состояние запросов в друзья
                    socketListener.getSendMessage()?.loadFriendsRequests()
                }
            }
        }
    }
    private val friendRequestsMutes = Mutex()
    private val actionsFriendsRequestsListener = object : EventListener<List<FriendRequestDto>> {
        override fun onEvent(event: List<FriendRequestDto>) {
            viewModelScope.launch {
                friendRequestsMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.FRIENDS_REQUESTS.ordinal)!!
                    var friendRequests = event.filter { it.updateTime > state.timePoint }.sortedBy { it.updateTime }
                    for (friendRequest in friendRequests) {
                        if (friendRequest.deleted) {
                            friendRequestDao.deleteById(friendRequest.userId)
                        } else {
                            friendRequestDao.insert(friendRequest)
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.updateTime }
                        statesTimePointsDao.insert(StateTimePoint(StateTypes.FRIENDS_REQUESTS.ordinal, maxTimePoint))
                    }
                }
            }
        }
    }
    private val initFriendsRequests = object: EventListener<FriendsRequestsInitializationState> {
        override fun onEvent(event: FriendsRequestsInitializationState) {
            viewModelScope.launch {
                friendRequestDao.clearAll()
                friendRequestDao.insertAll(event.friends)
                statesTimePointsDao.insert(StateTimePoint(StateTypes.FRIENDS_REQUESTS.ordinal, event.timePoint))
            }
            socketListener.getReceiveMessage().friendRequestsBus.removeListener(this)
        }
    }


    // Сообщения
    private val refreshMessagesListener = object : EventListener<Unit> {
        override fun onEvent(event: Unit) {
            viewModelScope.launch {
                val state = statesTimePointsDao.getStateTimePointByType(StateTypes.MESSAGES.ordinal)
                if (state != null) {
                    socketListener.getSendMessage()?.messagesActions(state)
                } else {
                    socketListener.getSendMessage()?.initMessages(InitMessenger(
                        size = 50
                    ))
                }
            }
        }
    }
    private val messagesMutes = Mutex()
    private val actionsMessagesListener = object : EventListener<List<MessageResponse>> {
        override fun onEvent(event: List<MessageResponse>) {
            val myId = profileState.getUserId()
            viewModelScope.launch {
                messagesMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.MESSAGES.ordinal)!!
                    var messages = event.filter { it.updateTime > state.timePoint }.sortedBy { it.updateTime }
                    for (message in messages) {
                        if (message.deleted == false) {
                            messageDao.insert(message)
                            // показать уведомление если надо
                            toastsState.newMessage(message, viewModelScope)

                            // если это мое сообщение то оно всегда считается прочитанным
                            if (message.senderId == myId) {
                                continue
                            }

                            // если чат с этим пользователем открыт то не увеличивать счетчик
                            val opponentId = message.getOpponentId(myId)
                            val openedChat = chatState.chat.value
                            if (openedChat != null && openedChat.getChatWithId(myId) == opponentId) {
                                continue
                            }

                            // увеличить счетчик непрочитанных сообщений
                            val currentUnread = unreadMessagesDao.findByUserId(opponentId)
                            if (currentUnread == null) {
                                unreadMessagesDao.insert(UnreadMessages(userId = opponentId, count = 1))
                            } else {
                                unreadMessagesDao.insert(UnreadMessages(userId = opponentId, count = currentUnread.count + 1))
                            }

                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.updateTime }
                        statesTimePointsDao.insert(StateTimePoint(StateTypes.MESSAGES.ordinal, maxTimePoint))
                    }
                }
            }
        }
    }
    private val initMessagesListener = object: EventListener<InitMessagesResponse> {
        override fun onEvent(event: InitMessagesResponse) {
            viewModelScope.launch {
                messageDao.clearAll()
                messageDao.insertAll(event.messages)
                statesTimePointsDao.insert(StateTimePoint(StateTypes.MESSAGES.ordinal, event.timePoint))
            }
            socketListener.getReceiveMessage().getReceiveMessenger().messagesInitBus.removeListener(this)
        }
    }
    private val messagesListener = object: EventListener<ResponseMessages> {
        override fun onEvent(event: ResponseMessages) {
            viewModelScope.launch {
                // добавляем дозагруженные сообщения в базу данных
                // это сообщения более старые которые пользователь хочет посмотреть и которых нету в локальной базе
                messageDao.insertAll(event.messages)
            }
        }
    }

    // Чаты
    private val refreshChatsListener = object : EventListener<Unit> {
        override fun onEvent(event: Unit) {
            viewModelScope.launch {
                val state = statesTimePointsDao.getStateTimePointByType(StateTypes.CHATS.ordinal)
                if (state != null) {
                    // если нету состояния то его инициализирует другой компонент
                    socketListener.getSendMessage()?.chatsActions(state)
                } else {
                    socketListener.getSendMessage()?.initChats(InitMessenger(
                        size = MessagesConsts.batchSize
                    ))
                }
            }
        }
    }
    private val chatsMutes = Mutex()
    private val actionsChatsListener = object : EventListener<List<ChatResponse>> {
        override fun onEvent(event: List<ChatResponse>) {
            viewModelScope.launch {
                chatsMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.CHATS.ordinal)!!
                    var chats = event.filter { it.updateTime > state.timePoint }.sortedBy { it.updateTime }
                    for (chat in chats) {
                        if (chat.deleted) {
                            chatDao.deleteById(chat.id!!)
                            messageDao.deleteByFirstUserIdAndSecondUserId(chat.firstUserId, chat.secondUserId)
                        } else {
                            chatDao.insert(chat)
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.updateTime }
                        statesTimePointsDao.insert(StateTimePoint(StateTypes.CHATS.ordinal, maxTimePoint))
                    }
                }
            }
        }
    }
    private val initChatsListener = object: EventListener<InitChatsResponse> {
        override fun onEvent(event: InitChatsResponse) {
            viewModelScope.launch {
                chatDao.clearAll()
                chatDao.insertAll(event.chats)
                statesTimePointsDao.insert(StateTimePoint(StateTypes.CHATS.ordinal, event.timePoint))
            }
            socketListener.getReceiveMessage().getReceiveMessenger().chatsInitBus.removeListener(this)
        }
    }


    @OptIn(ExperimentalSerializationApi::class)
    private val pingListener = object: EventListener<Unit> {
        override fun onEvent(event: Unit) {
            // отвечаем серверу что мы живы
            socketListener.getSendMessage()?.sendMessage(
                SocketBinaryMessage("pong", byteArrayOf())
            )
        }
    }
    private val connectionClosedListener = object : EventListener<String> {
        override fun onEvent(event: String) {
            socketConnect.disconnect(event)
        }
    }

    init {
        // Если соединение с сервером установлено то запрашиваем данные обновления состояния
        socketConnectionState.socketStateBus.addListener(object: EventListener<SocketConnectionStates> {
            override fun onEvent(event: SocketConnectionStates) {

                when (event) {
                    // соединение было открыто или переоткрыто
                    SocketConnectionStates.OPEN -> {
                        // запрашиваем у сервера историю для синхронизации
                        refreshFriendsListener.onEvent(Unit)
                        refreshFriendsRequestsListener.onEvent(Unit)
                        refreshChatsListener.onEvent(Unit)
                        refreshMessagesListener.onEvent(Unit)
                        refreshAvatars()


                        // отправляем сообщения которые не были отправлены
                        // и не получили подтверждения
                        viewModelScope.launch {
                            val unsentMessages = messageDao.getUnsentMessages()
                            for (message in unsentMessages) {

                                // проверяем payload на наличие картинок
                                val payload = message.getClearedPayload()
                                if (payload.isNotEmpty()) {
                                    // если есть незагруженные на сервер картинки то не надо отправлять
                                    val statuses = imageUploadingStatusDao.findByTempIdIn(payload)
                                    if (statuses.any { it.status != UploadingImageStatus.UPLOADED.ordinal }) {
                                        continue
                                    }
                                }

                                // отправить сообщение еще раз
                                // payload точно загружен на сервер
                                chatState.resendMessage(viewModelScope, message)
                            }
                        }

                        // отправляем ключ фаербейса еще раз если он был
                        firebaseState.resaveSame()

                        // запрашиваем сатусы друзей. Кто онлайн, а кто оффлайн
                        socketListener.getSendMessage()?.requestOnlineFriendsStatuses()
                    }
                    SocketConnectionStates.CLOSED -> {
                        // Если соединение было закрыто по причине нарушения политики безопасности
                        // Неправильный или невалидный токен
                        val closedStatus = socketConnectionState.getCloseStatus()
                        if (closedStatus == 1008) {
                            // разлогиниваем пользователя
                            viewModelScope.launch {
                                authenticationState.logout(applicationContext)

                                // если токен невалидный то сокрее всего я базу почистил
                                // и поэтому нужно еще раз инициализировать состояния
                                // всю локальную базу чистим
                                friendDao.clearAll()
                                friendRequestDao.clearAll()
                                chatDao.clearAll()
                                messageDao.clearAll()
                            }


                        }
                    }
                    SocketConnectionStates.FAILED -> {}
                    SocketConnectionStates.WAIT_TO_RECONNECT -> {}
                    SocketConnectionStates.CONNECTING -> {}
                }
            }
        })

        // Слушаем закрытие соединения
        socketListener.getReceiveMessage().connectionClosedBus.addListener(connectionClosedListener)


        // Друзья Следим за состояними
        socketListener.getReceiveMessage().refreshFriendsBus.addListener(refreshFriendsListener)
        socketListener.getReceiveMessage().friendsActionsBus.addListener(actionsFriendsListener)
        socketListener.getReceiveMessage().friendsBus.addListener(initFriendsListener)

        // Запросы в друзья
        socketListener.getReceiveMessage().refreshFriendsRequestsBus.addListener(refreshFriendsRequestsListener)
        socketListener.getReceiveMessage().friendsRequestsActionsBus.addListener(actionsFriendsRequestsListener)
        socketListener.getReceiveMessage().friendRequestsBus.addListener(initFriendsRequests)

        // Сообщения
        socketListener.getReceiveMessage().getReceiveMessenger().refreshMessages.addListener(refreshMessagesListener)
        socketListener.getReceiveMessage().getReceiveMessenger().messagesActionsBus.addListener(actionsMessagesListener)
        socketListener.getReceiveMessage().getReceiveMessenger().messagesInitBus.addListener(initMessagesListener)
        socketListener.getReceiveMessage().getReceiveMessenger().messagesBus.addListener(messagesListener)

        // Чаты
        socketListener.getReceiveMessage().getReceiveMessenger().refreshChats.addListener(refreshChatsListener)
        socketListener.getReceiveMessage().getReceiveMessenger().chatsActionsBus.addListener(actionsChatsListener)
        socketListener.getReceiveMessage().getReceiveMessenger().chatsInitBus.addListener(initChatsListener)

        // пинги от сервера чтобы понять когда сессия мертва
        socketListener.getReceiveMessage().pingBus.addListener(pingListener)
    }
}