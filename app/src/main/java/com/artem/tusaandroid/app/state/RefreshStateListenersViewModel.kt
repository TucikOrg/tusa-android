package com.artem.tusaandroid.app.state

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.dto.AvatarAction
import com.artem.tusaandroid.dto.AvatarActionType
import com.artem.tusaandroid.dto.FriendActionDto
import com.artem.tusaandroid.dto.FriendRequestActionDto
import com.artem.tusaandroid.dto.FriendsActionType
import com.artem.tusaandroid.dto.FriendsInitializationState
import com.artem.tusaandroid.dto.FriendsRequestsInitializationState
import com.artem.tusaandroid.dto.messenger.ChatAction
import com.artem.tusaandroid.dto.messenger.ChatsActionType
import com.artem.tusaandroid.dto.messenger.InitChatsResponse
import com.artem.tusaandroid.dto.messenger.InitMessagesResponse
import com.artem.tusaandroid.dto.messenger.InitMessenger
import com.artem.tusaandroid.dto.messenger.MessagesActionType
import com.artem.tusaandroid.dto.messenger.MessagesAction
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.FriendRequestDao
import com.artem.tusaandroid.room.StateTimePoint
import com.artem.tusaandroid.room.StateTypes
import com.artem.tusaandroid.room.StatesTimePointDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketConnectionStates
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.launch
import kotlinx.coroutines.sync.Mutex
import kotlinx.coroutines.sync.withLock
import java.time.LocalDateTime
import java.time.ZoneOffset
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
    private val locationState: LocationsState
): ViewModel() {

    // Аватарки
    private val refreshAvatarsListener = object : EventListener<Unit> {
        override fun onEvent(event: Unit) {
            viewModelScope.launch {
                var state = statesTimePointsDao.getStateTimePointByType(StateTypes.AVATARS.ordinal)
                if (state == null) {
                    statesTimePointsDao.insert(StateTimePoint(StateTypes.AVATARS.ordinal, LocalDateTime.now().toEpochSecond(ZoneOffset.UTC)))
                    state = statesTimePointsDao.getStateTimePointByType(StateTypes.AVATARS.ordinal)
                }
                socketListener.getSendMessage()?.avatarsActions(state!!)
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
                        size = 50
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

    init {
        // Если соединение с сервером установлено то запрашиваем данные обновления состояния
        socketConnectionState.socketStateBus.addListener(object: EventListener<SocketConnectionStates> {
            override fun onEvent(event: SocketConnectionStates) {
                // запрашиваем у сервера историю для синхронизации
                if (event == SocketConnectionStates.OPEN) {
                    refreshFriendsListener.onEvent(Unit)
                    refreshFriendsRequestsListener.onEvent(Unit)
                    refreshChatsListener.onEvent(Unit)
                    refreshMessagesListener.onEvent(Unit)
                }
            }
        })

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

        // Чаты
        socketListener.getReceiveMessage().getReceiveMessenger().refreshChats.addListener(refreshChatsListener)
        socketListener.getReceiveMessage().getReceiveMessenger().chatsActionsBus.addListener(actionsChatsListener)
        socketListener.getReceiveMessage().getReceiveMessenger().chatsInitBus.addListener(initChatsListener)

        // Аватарки
        socketListener.getReceiveMessage().refreshAvatarsBus.addListener(refreshAvatarsListener)
        socketListener.getReceiveMessage().avatarsActionsBus.addListener(actionsAvatarsListener)
    }
}