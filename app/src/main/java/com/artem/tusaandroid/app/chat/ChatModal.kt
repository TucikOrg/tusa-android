package com.artem.tusaandroid.app.chat

import android.graphics.Bitmap
import android.net.Uri
import androidx.compose.foundation.Image
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
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.rememberLazyListState
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.foundation.text.selection.SelectionContainer
import androidx.compose.material3.CircularProgressIndicator
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.produceState
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.Alignment
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.lerp
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.tooling.preview.Preview
import coil.compose.AsyncImage
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.friends.FriendAvatar
import com.artem.tusaandroid.app.action.friends.PreviewFriendAvatarViewModel
import com.artem.tusaandroid.app.systemui.IsLightGlobal
import com.artem.tusaandroid.dto.MessageResponse
import com.artem.tusaandroid.isPreview
import kotlinx.coroutines.delay
import java.time.Instant
import java.time.LocalDateTime
import java.time.ZoneId
import java.time.ZoneOffset
import java.time.format.DateTimeFormatter


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

    val isOnline = chatViewModel.getIsFriendOnline()

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
            Row(
                verticalAlignment = Alignment.CenterVertically
            ) {
                Text(text = name, style = MaterialTheme.typography.bodyLarge)
                Spacer(modifier = Modifier.weight(1f))
                val color = if (isOnline.value) lerp(Color.Green, Color.Black, 0.2f) else Color.Gray
                val localDateTime = Instant.ofEpochSecond(friend.lastOnlineTime)
                    .atZone(ZoneId.systemDefault())
                    .toLocalDateTime()
                val time = DateTimeFormatter.ofPattern("HH:mm MM.dd").format(localDateTime)
                Text(
                    text = if (isOnline.value) "В сети" else "Был(а) в сети $time",
                    color = color,
                    style = MaterialTheme.typography.bodySmall
                )
                Spacer(modifier = Modifier.width(5.dp))
                Box(
                    modifier = Modifier
                        .size(15.dp)
                        .background(color, shape = CircleShape)
                )
            }
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

    val lazyListState = rememberLazyListState()
    val isAtEnd = remember {
        derivedStateOf {
            val visibleItems = lazyListState.layoutInfo.visibleItemsInfo
            val lastVisibleItemIndex = visibleItems.lastOrNull()?.index
            if (lastVisibleItemIndex == null) {
                return@derivedStateOf false
            }

            lastVisibleItemIndex > messages.size - 4
        }
    }

    // Load more items when reaching the end
    LaunchedEffect(isAtEnd.value) {
        if (isAtEnd.value) {
            chatViewModel.loadMoreItems()
        }
    }

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
            key = { index -> messages[index].temporaryId }
        ) { messageIndex ->
            val message = messages[messageIndex]
            MessageItem(message, chatViewModel.chatsState.getUserId(), chatViewModel)
        }
    }

    LaunchedEffect(Unit) {
        while (true) {
            delay(5000)
            chatViewModel.checkWritingMessagesLife()
            //Log.i("ChatModal", "Check writing messages life")
        }
    }

    val writingMessage = chatViewModel.getCurrentWritingMessage()
    if (writingMessage.value.isNotEmpty()) {
        WritingMessageOnlineShow(writingMessage)
    }

}

@Composable
fun WritingMessageOnlineShow(message: MutableState<String>) {
    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 8.dp, vertical = 4.dp),
        horizontalArrangement = Arrangement.Start
    ) {
        Box(
            modifier = Modifier
                .background(
                    color = lerp(MaterialTheme.colorScheme.surface, Color.White, 0.3f),
                    shape = RoundedCornerShape(8.dp)
                )
                .padding(8.dp),
        ) {
            Text(
                modifier = Modifier.padding(bottom = 20.dp, end = 15.dp),
                text = message.value,
                style = MaterialTheme.typography.bodyLarge,
                color = MaterialTheme.colorScheme.primary
            )
            Row(
                modifier = Modifier.align(Alignment.BottomEnd),
                horizontalArrangement = Arrangement.End,
                verticalAlignment = Alignment.Bottom
            ) {
                TypingIndicator()
                Spacer(modifier = Modifier.width(4.dp))
                Icon(
                    imageVector = ImageVector.vectorResource(id = R.drawable.edit),
                    contentDescription = "Edit",
                    modifier = Modifier.size(24.dp),
                )
            }
        }
    }

}

@Composable
fun MessageItem(message: MessageResponse, userId: Long, chatViewModel: ChatViewModel) {
    val isMyMessage = message.senderId == userId

    Row(
        modifier = Modifier
            .fillMaxWidth()
            .padding(horizontal = 8.dp, vertical = 4.dp),
        horizontalArrangement = if (isMyMessage) Arrangement.End else Arrangement.Start
    ) {
        Column(
            modifier = Modifier
                .fillMaxWidth(0.8f)
                .background(
                    color = if (isMyMessage) MaterialTheme.colorScheme.surface else lerp(MaterialTheme.colorScheme.surface, Color.White, 0.3f),
                    shape = RoundedCornerShape(8.dp)
                )
                .padding(8.dp),
        ) {
            SelectionContainer {
                Text(
                    modifier = Modifier.padding(bottom = 20.dp, end = 15.dp),
                    text = message.message,
                    style = MaterialTheme.typography.bodyLarge,
                    color = if (isMyMessage) MaterialTheme.colorScheme.primary else MaterialTheme.colorScheme.primary
                )
            }
            Spacer(modifier = Modifier.height(4.dp))

            if (message.payload.isNotEmpty()) {
                for (tempFileId in message.payload.split(",")) {
                    chatViewModel.Image(tempFileId)
                }
                Spacer(modifier = Modifier.height(10.dp))
            }
            val localDateTime = Instant.ofEpochSecond(message.creation)
                .atZone(ZoneId.systemDefault())
                .toLocalDateTime()
            val time = DateTimeFormatter.ofPattern("HH:mm").format(localDateTime)
            Row (
                verticalAlignment = Alignment.CenterVertically,
                horizontalArrangement = if (isMyMessage) Arrangement.Start else Arrangement.End
            ) {
                val serverUploaded = message.isServerUploaded()
                if (serverUploaded == false) {
                    CircularProgressIndicator(
                        modifier = Modifier.size(10.dp),
                        strokeWidth = 2.dp,
                    )
                    Spacer(modifier = Modifier.width(2.dp))
                }

                Text(
                    text = time,
                    style = MaterialTheme.typography.bodySmall,
                    color = Color.Gray
                )
            }
        }
    }
}






