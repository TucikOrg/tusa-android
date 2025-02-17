package com.artem.tusaandroid.app.state

import android.content.Context
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.app.chat.MessagesConsts
import com.artem.tusaandroid.dto.AvatarAction
import com.artem.tusaandroid.dto.AvatarActionType
import com.artem.tusaandroid.dto.FriendActionDto
import com.artem.tusaandroid.dto.FriendRequestActionDto
import com.artem.tusaandroid.dto.FriendsActionType
import com.artem.tusaandroid.dto.FriendsInitializationState
import com.artem.tusaandroid.dto.FriendsRequestsInitializationState
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.dto.ResponseMessages
import com.artem.tusaandroid.dto.messenger.ChatAction
import com.artem.tusaandroid.dto.messenger.ChatsActionType
import com.artem.tusaandroid.dto.messenger.InitChatsResponse
import com.artem.tusaandroid.dto.messenger.InitMessagesResponse
import com.artem.tusaandroid.dto.messenger.InitMessenger
import com.artem.tusaandroid.dto.messenger.MessagesActionType
import com.artem.tusaandroid.dto.messenger.MessagesAction
import com.artem.tusaandroid.firebase.FirebaseState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.FriendRequestDao
import com.artem.tusaandroid.room.StateTimePoint
import com.artem.tusaandroid.room.StateTypes
import com.artem.tusaandroid.room.StatesTimePointDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketConnect
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketConnectionStates
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import dagger.hilt.android.qualifiers.ApplicationContext
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
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
    @ApplicationContext private val applicationContext: Context
): ViewModel() {

    // Аватарки
    private val refreshAvatarsListener = object : EventListener<Long> {
        override fun onEvent(serverTimePoint: Long) {
            viewModelScope.launch {
                val state = statesTimePointsDao.getStateTimePointByType(StateTypes.AVATARS.ordinal)
                if (state != null) {
                    socketListener.getSendMessage()?.avatarsActions(state)
                    return@launch
                }

                val stateTimePoint = StateTimePoint(StateTypes.AVATARS.ordinal, serverTimePoint)
                statesTimePointsDao.insert(stateTimePoint)
                socketListener.getSendMessage()?.avatarsActions(stateTimePoint)
            }
        }
    }
    private val avatarMutes = Mutex()
    private val actionsAvatarsListener = object : EventListener<List<AvatarAction>> {
        override fun onEvent(event: List<AvatarAction>) {
            viewModelScope.launch {
                avatarMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.AVATARS.ordinal)!!
                    var useActions = event.filter { it.actionTime > state.timePoint }.sortedBy { it.actionTime }
                    for (action in useActions) {
                        when (action.actionType) {
                            AvatarActionType.CHANGE -> {
                                // перезагрузить аватрки юзеров
                                avatarState.retrieveAvatar(action.ownerId, viewModelScope,
                                    forceReload = true
                                ) {
                                    viewModelScope.launch {
                                        statesTimePointsDao.insert(StateTimePoint(StateTypes.AVATARS.ordinal, action.actionTime))
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
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
    private val actionsFriendsListener = object : EventListener<List<FriendActionDto>> {
        override fun onEvent(event: List<FriendActionDto>) {
            viewModelScope.launch {
                friendMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.FRIENDS.ordinal)!!
                    var useActions = event.filter { it.actionTime > state.timePoint }.sortedBy { it.actionTime }
                    for (action in useActions) {
                        when (action.friendsActionType) {
                            FriendsActionType.ADD -> {
                                friendDao.insert(action.friendDto)
                            }
                            FriendsActionType.DELETE -> {
                                friendDao.deleteById(action.friendDto.id)
                                locationState.removeLocation(action.friendDto.id)
                            }
                            FriendsActionType.CHANGE -> {
                                friendDao.insert(action.friendDto)
                            }
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.actionTime }
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
    private val actionsFriendsRequestsListener = object : EventListener<List<FriendRequestActionDto>> {
        override fun onEvent(event: List<FriendRequestActionDto>) {
            viewModelScope.launch {
                friendRequestsMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.FRIENDS_REQUESTS.ordinal)!!
                    var useActions = event.filter { it.actionTime > state.timePoint }.sortedBy { it.actionTime }
                    for (action in useActions) {
                        when (action.friendsActionType) {
                            FriendsActionType.ADD -> {
                                friendRequestDao.insert(action.friendRequestDto)
                            }
                            FriendsActionType.DELETE -> {
                                friendRequestDao.deleteById(action.friendRequestDto.userId)
                            }
                            FriendsActionType.CHANGE -> {
                                //friendRequestDao.insert(action.friendRequestDto)
                            }
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.actionTime }
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
                    // если нету состояния то его инициализирует другой компонент
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
    private val actionsMessagesListener = object : EventListener<List<MessagesAction>> {
        override fun onEvent(event: List<MessagesAction>) {
            viewModelScope.launch {
                messagesMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.MESSAGES.ordinal)!!
                    var useActions = event.filter { it.actionTime > state.timePoint }.sortedBy { it.actionTime }
                    for (action in useActions) {
                        when (action.actionType) {
                            MessagesActionType.ADD -> {
                                messageDao.insert(action.message)
                            }
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.actionTime }
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
    private val actionsChatsListener = object : EventListener<List<ChatAction>> {
        override fun onEvent(event: List<ChatAction>) {
            viewModelScope.launch {
                chatsMutes.withLock {
                    val state = statesTimePointsDao.getStateTimePointByType(StateTypes.CHATS.ordinal)!!
                    var useActions = event.filter { it.actionTime > state.timePoint }.sortedBy { it.actionTime }
                    for (action in useActions) {
                        when (action.actionType) {
                            ChatsActionType.ADD -> {
                                chatDao.insert(action.chat)
                            }
                            ChatsActionType.DELETE -> {
                                // если чат удаляем то удаляем и все сообщения
                                val chat = action.chat
                                chatDao.deleteById(chat.id!!)
                                messageDao.deleteByFirstUserIdAndSecondUserId(chat.firstUserId, chat.secondUserId)
                            }
                            ChatsActionType.CHANGE -> {
                                chatDao.insert(action.chat)
                            }
                        }
                    }
                    if (event.isNotEmpty()) {
                        val maxTimePoint = event.maxOf { it.actionTime }
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

                        // отправляем сообщения которые не были отправлены
                        // и не получили подтверждения
                        viewModelScope.launch {
                            val unsentMessages = messageDao.getUnsentMessages()
                            for (message in unsentMessages) {
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

        // Аватарки
        socketListener.getReceiveMessage().refreshAvatarsBus.addListener(refreshAvatarsListener)
        socketListener.getReceiveMessage().avatarsActionsBus.addListener(actionsAvatarsListener)
    }
}