package com.artem.tusaandroid.app.action.chats

import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
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
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.derivedStateOf
import androidx.compose.runtime.mutableStateListOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.action.friends.FriendAvatar
import com.artem.tusaandroid.app.action.friends.PreviewFriendViewModel
import com.artem.tusaandroid.dto.ChatResponse
import com.artem.tusaandroid.isPreview

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ChatsActionFab(modifier: Modifier, model: ChatsViewModel) {
    if (model.showModal.value) {
        val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = false)
        ModalBottomSheet(
            sheetState = sheetState,
            shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
            onDismissRequest = {
                model.dismiss()
            },
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing),
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = false
            ),
        ) {

            LaunchedEffect(Unit) {
                model.loadChats()
            }

            val chats = model.getChats()
            val listState = model.lazyListState
            val isAtEnd = remember {
                derivedStateOf {
                    val visibleItems = listState.layoutInfo.visibleItemsInfo
                    val lastVisibleItemIndex = visibleItems.lastOrNull()?.index ?: -1
                    lastVisibleItemIndex == chats.value.size - 1
                }
            }
            LaunchedEffect(isAtEnd.value) {
                if (isAtEnd.value) {
                    model.loadChats()
                }
            }

            LazyColumn(
                modifier = modifier.fillMaxWidth(),
                state = model.lazyListState
            ) {
                item {
                    Text(
                        text = "Чаты",
                        style = MaterialTheme.typography.headlineMedium,
                        fontWeight = FontWeight.Bold,
                        modifier = Modifier.padding(8.dp)
                    )
                }
                items(
                    count = chats.value.size,
                    key = { index -> chats.value[index].chatId }
                ) { index ->
                    ChatItem(chats.value[index], model)
                }
                item {
                    Spacer(modifier = Modifier.height(50.dp))
                }
            }

        }
    }

    FloatingActionButton(
        onClick = {
            model.showModal.value = true
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.chat), contentDescription = "Чаты")
    }
}

@Composable
fun ChatItem(chat: ChatResponse, model: ChatsViewModel) {
    ElevatedButton(
        modifier = Modifier.fillMaxSize(),
        shape = RoundedCornerShape(0.dp),
        onClick = {
            model.openChat(chat)
        }
    ) {
        Row(
            modifier = Modifier
                .fillMaxSize(),
            verticalAlignment = Alignment.CenterVertically
        ) {
            val friend = model.getChatFriend(chat)

            FriendAvatar(
                modifier = Modifier
                    .size(70.dp)
                    .padding(0.dp),
                userId = chat.toId,
                model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendViewModel())
            )

            Spacer(modifier = Modifier.width(20.dp))

            Column(
                modifier = Modifier.weight(1f),
                verticalArrangement = Arrangement.Center
            ) {
                Text(
                    text = friend?.name?: "",
                    style = MaterialTheme.typography.headlineSmall,
                )
                Spacer(modifier = Modifier.height(3.dp))
                Text(
                    text = chat.lastMessage,
                    style = MaterialTheme.typography.bodyLarge,
                    maxLines = 1,
                    color = Color.Gray
                )
            }
        }
    }

}