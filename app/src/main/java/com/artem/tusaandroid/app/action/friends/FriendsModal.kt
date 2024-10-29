package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.lazy.LazyColumn
import androidx.compose.foundation.lazy.items
import androidx.compose.material3.Badge
import androidx.compose.material3.BadgedBox
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.isPreview

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FriendsModal(model: FriendViewModel) {
    val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
    ModalBottomSheet(
        modifier = Modifier.navigationBarsPadding(),
        sheetState = sheetState,
        onDismissRequest = { model.showModal.value = false },
    ) {
        val friends = model.getFriends()
        val requests = model.getRequests()

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
                    if (requests.value.isNotEmpty()) {
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
                                    Badge {
                                        Text(requests.value.size.toString())
                                    }
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
                    for (request in requests.value) {
                        RequestToFriends(
                            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewRequestToFriendViewModel()),
                            friend = FriendDto(
                                id = request.id,
                                name = request.name,
                                uniqueName = request.uniqueName,
                            )
                        )
                        Spacer(modifier = Modifier.height(10.dp))
                    }
                    Spacer(modifier = Modifier.height(20.dp))
                }
                items(friends.value) { friend ->
                    FriendRow(
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendRowViewModel()),
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
                    friendsVM = model
                )
            }
            Spacer(modifier = Modifier.height(60.dp))
        }
    }
}