package com.artem.tusaandroid.app.chat

import androidx.compose.runtime.setValue
import androidx.compose.runtime.getValue
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.interaction.MutableInteractionSource
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.ExperimentalLayoutApi
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.Alignment
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.platform.LocalFocusManager
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.friends.FriendAvatar
import com.artem.tusaandroid.app.action.friends.PreviewFriendAvatarViewModel
import com.artem.tusaandroid.app.systemui.IsLightGlobal
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.isPreview
import java.text.SimpleDateFormat
import java.util.Locale


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
                isAppearanceLightStatusBars = !IsLightGlobal.isLight
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
    LaunchedEffect(Unit) {
        chatViewModel.getChatFriend()
    }

    val friend = chatViewModel.chatFriend.value
    if (friend == null) return

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(8.dp),
        verticalAlignment = Alignment.CenterVertically
    ) {
        FriendAvatar(
            modifier = Modifier.size(50.dp),
            userId = friend.id,
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
    val messages by chatViewModel.getMessages().collectAsState()

//    val isAtEnd = remember {
//        derivedStateOf {
//            val visibleItems = chatViewModel.lazyListState.layoutInfo.visibleItemsInfo
//            val lastVisibleItemIndex = visibleItems.lastOrNull()?.index ?: -Int.MAX_VALUE
//            lastVisibleItemIndex > messages.value.size - 10
//        }
//    }

    // Load more items when reaching the end
//    LaunchedEffect(isAtEnd.value) {
//        if (isAtEnd.value) {
//            chatViewModel.loadMoreItems()
//        }
//    }

    val lazyListState = rememberLazyListState()
    LaunchedEffect(messages) {
        if (messages.isNotEmpty()) {
            val visibleItems = lazyListState.layoutInfo.visibleItemsInfo
            if (visibleItems.isEmpty()) {
                return@LaunchedEffect
            }
            val isFirstMessage = visibleItems.first().index <= 1
            if (isFirstMessage) {
                lazyListState.animateScrollToItem(0)
            }
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
        state = lazyListState,
    ) {
        items(
            count = messages.size,
            key = { index -> messages[index].creation }
        ) { messageIndex ->
            val message = messages[messageIndex]
            MessageItem(message, chatViewModel.chatsState.getUserId())
        }
    }
}

@Composable
fun MessageItem(message: MessageResponse, userId: Long) {
    val isMyMessage = message.senderId == userId
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





