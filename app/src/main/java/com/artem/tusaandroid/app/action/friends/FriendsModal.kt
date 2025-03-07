package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Badge
import androidx.compose.material3.BadgedBox
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.systemui.IsLightGlobal
import com.artem.tusaandroid.isPreview
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.beauty.NewMessagesBadge
import com.artem.tusaandroid.dto.FriendDto

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FriendsModal(model: FriendViewModel) {
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    ModalBottomSheet(
        shape = RoundedCornerShape(topEnd = 10.dp, topStart = 10.dp),
        sheetState = sheetState,
        onDismissRequest = {
            model.showModal.value = false
        },
        properties = ModalBottomSheetProperties(
            isAppearanceLightStatusBars = !IsLightGlobal.isLight
        ),
        modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing)
    ) {
        val requests by model.requests.collectAsState()
        val friends by model.friends.collectAsState()

        Column(
            modifier = Modifier.fillMaxSize(),
            horizontalAlignment = Alignment.CenterHorizontally
        ) {
            Text(
                modifier = Modifier.fillMaxWidth().padding(horizontal = 10.dp),
                text = "Друзья",
                style = MaterialTheme.typography.headlineSmall,
                fontWeight = FontWeight.Bold,
                textAlign = TextAlign.Start
            )
            Spacer(modifier = Modifier.height(10.dp))
            LazyColumn(
                modifier = Modifier.weight(1f)
            ) {
                item {
                    if (requests.isNotEmpty()) {
                        Row {
                            Text(
                                "Запросы в друзья",
                                style = MaterialTheme.typography.bodyMedium,
                                fontWeight = FontWeight.Bold,
                                modifier = Modifier.padding(horizontal = 10.dp)
                            )
                            Spacer(modifier = Modifier.width(5.dp))
                            BadgedBox(
                                badge = {
                                    NewMessagesBadge(
                                        unreadMessages = requests.size,
                                        modifier = Modifier
                                    )
                                }
                            ) {
                                Icon(
                                    imageVector = ImageVector.vectorResource(id = R.drawable.group),
                                    contentDescription = "Friends requests",
                                )
                            }
                        }
                    }
                    Spacer(modifier = Modifier.height(10.dp))
                    for (request in requests) {
                        RequestToFriends(
                            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewRequestToFriendViewModel()),
                            friend = FriendDto(
                                id = request.userId,
                                name = request.userName,
                                uniqueName = request.userUniqueName,
                                lastOnlineTime = 0,
                                updateTime = request.updateTime,
                                deleted = request.deleted
                            )
                        )
                        Spacer(modifier = Modifier.height(10.dp))
                    }
                    Spacer(modifier = Modifier.height(20.dp))
                }
                items(friends) { friend ->
                    FriendRow(
                        model = hiltViewModel(),
                        friend = friend
                    )
                    Spacer(modifier = Modifier.height(10.dp))
                }
            }
            Spacer(modifier = Modifier.height(10.dp))
            Row(
                modifier = Modifier.fillMaxWidth().padding(horizontal = 10.dp),
                horizontalArrangement = Arrangement.End,
                verticalAlignment = Alignment.CenterVertically
            ) {
                FindFriendButton(
                    model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFindFriendViewModel()),
                )
            }
        }
    }
}