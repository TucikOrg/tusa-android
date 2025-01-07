package com.artem.tusaandroid.app.chat

import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.gestures.detectVerticalDragGestures
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.asPaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.ime
import androidx.compose.foundation.layout.imeNestedScroll
import androidx.compose.foundation.layout.imePadding
import androidx.compose.foundation.layout.isImeVisible
import androidx.compose.foundation.layout.navigationBars
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeContent
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.statusBars
import androidx.compose.foundation.layout.systemBars
import androidx.compose.foundation.layout.systemBarsPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.LazyListState
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.rememberScrollState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.KeyboardActions
import androidx.compose.foundation.verticalScroll
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.OutlinedTextField
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TopAppBarDefaults
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.SideEffect
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.Alignment
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.tooling.preview.Preview
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.friends.FriendAvatar
import com.artem.tusaandroid.app.action.friends.PreviewFriendAvatarViewModel
import com.artem.tusaandroid.dto.ChatResponse
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.isPreview
import com.google.accompanist.systemuicontroller.rememberSystemUiController
import java.text.SimpleDateFormat
import java.util.Locale

@Preview()
@Composable
fun ChatModalPreview() {
    ChatModal(chatViewModel = ChatViewModelPreview())
}

@Preview
@Composable
fun ChatPreview() {
    ChatWindow(
        chatViewModel = ChatViewModelPreview()
    )
}

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ChatModal(chatViewModel: ChatViewModel) {
    if (chatViewModel.getModalOpened().value) {
        val sheetState = rememberModalBottomSheetState(
            skipPartiallyExpanded = true,
        )

        ModalBottomSheet (
            sheetState = sheetState,
            shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
            onDismissRequest = {
                chatViewModel.closeChat()
            },
            scrimColor = Color.Transparent,
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing),
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = false
            ),
        ) {
            ChatWindow(
                chatViewModel = chatViewModel
            )
        }
    }
}

@OptIn(ExperimentalLayoutApi::class)
@Composable
fun ChatWindow(
    chatViewModel: ChatViewModel
) {
    Column(
        modifier = Modifier
    ) {
        TopPanel(chatViewModel)
        MessagesArea(
            modifier = Modifier.weight(1f),
            chatViewModel = chatViewModel
        )
        ChatInput(chatViewModel = chatViewModel)
    }
}

@Composable
fun TopPanel(chatViewModel: ChatViewModel) {
    val friend = chatViewModel.getChatFriend()!!
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(8.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        FriendAvatar(
            modifier = Modifier.size(50.dp),
            userId = friend.id?: 0,
            model = TucikViewModel(
                preview = chatViewModel.isPreview(),
                previewModel = PreviewFriendAvatarViewModel()
            )
        )

        Spacer(modifier = Modifier.width(8.dp))

        // User Info
        var name = friend.name
        if (name.isEmpty()) {
            name = friend.uniqueName?: ""
        }
        Column(
            verticalArrangement = Arrangement.Top
        ) {
            Text(text = friend.name, style = MaterialTheme.typography.headlineMedium)
            if (name != friend.uniqueName) {
                Text(text = friend.uniqueName?: "", color = Color.Gray, style = MaterialTheme.typography.bodySmall)
            }
        }
    }
}

@OptIn(ExperimentalLayoutApi::class, ExperimentalMaterial3Api::class)
@Composable
fun MessagesArea(
    modifier: Modifier,
    chatViewModel: ChatViewModel
) {
    val focusManager = LocalFocusManager.current
    val mutableIndicationSource = remember { MutableInteractionSource()}
    val messages = chatViewModel.messages
    val chat = chatViewModel.getChat()
    val loading = chatViewModel.loading
    val closeChatCount = chatViewModel.getCloseChatCount()

    LaunchedEffect(closeChatCount) {
        chatViewModel.newChatOpened()
    }

    val isAtEnd = remember {
        derivedStateOf {
            val visibleItems = chatViewModel.lazyListState.layoutInfo.visibleItemsInfo
            val lastVisibleItemIndex = visibleItems.lastOrNull()?.index ?: -Int.MAX_VALUE
            lastVisibleItemIndex > messages.value.size - 10
        }
    }

    // Load more items when reaching the end
    LaunchedEffect(isAtEnd.value) {
        if (isAtEnd.value) {
            chatViewModel.loadMoreItems()
        }
    }

    LazyColumn(
        modifier = modifier.fillMaxWidth().clickable(
            indication = null,
            interactionSource = mutableIndicationSource,
            onClick = {
                focusManager.clearFocus()
            }
        ),
        reverseLayout = true,
        state = chatViewModel.lazyListState,
    ) {
        items(
            count = messages.value.size,
            key = { index -> messages.value[index].creation }
        ) { messageIndex ->
            val message = messages.value[messageIndex]
            if (message.chatId == -1L) {
                ChatLoadingPlaceholder()
            } else {
                MessageItem(message, chatViewModel.chatsState?.getUserId()?: 0)
            }
        }
    }
}


@Preview
@Composable
fun MessagePreview() {
    Column(
        modifier = Modifier.fillMaxSize()
    ) {
        MessageItem(
            message = MessageResponse(
                ownerId = 1,
                toId = 2,
                chatId = 1,
                payload = listOf(),
                message = "Hello",
                creation = System.currentTimeMillis(),
                deletedOwner = false,
                deletedTo = false,
                changed = false,
                read = false,
            ),
            userId = 1
        )
        MessageItem(
            message = MessageResponse(
                ownerId = 2,
                toId = 1,
                chatId = 1,
                payload = listOf(),
                message = "Hello",
                creation = System.currentTimeMillis(),
                deletedOwner = false,
                deletedTo = false,
                changed = false,
                read = false,
            ),
            userId = 1
        )
    }
}

@Composable
fun MessageItem(message: MessageResponse, userId: Long) {
    val isMyMessage = message.ownerId == userId
    val sdf = SimpleDateFormat("HH:mm", Locale.getDefault())
    val time = sdf.format(message.creation)
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 8.dp, vertical = 4.dp),
        horizontalArrangement = if (isMyMessage) Arrangement.End else Arrangement.Start
    ) {
        Box(
            modifier = Modifier
                .background(
                    color = if (isMyMessage) MaterialTheme.colorScheme.primary else Color.LightGray,
                    shape = RoundedCornerShape(8.dp)
                )
                .padding(8.dp),
        ) {
            Text(
                modifier = Modifier.padding(bottom = 20.dp, end = 15.dp),
                text = message.message,
                style = MaterialTheme.typography.bodyLarge,
                color = if (isMyMessage) Color.White else Color.Black
            )
            Text(
                modifier = Modifier.align(Alignment.BottomEnd),
                text = time,
                style = MaterialTheme.typography.bodySmall,
                color = Color.Gray
            )
        }
    }
}





