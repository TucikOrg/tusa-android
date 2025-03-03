package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.app.profile.ProfileUserColumn
import com.artem.tusaandroid.isPreview
import com.artem.tusaandroid.dto.FriendDto

@Composable
fun FriendRowInSearch(model: FriendRowInSearchViewModel, friend: FriendDto) {

    val height = 90.dp
    Box(
        modifier = Modifier
            .fillMaxWidth()
            .height(height)
    ) {
        ElevatedButton(
            modifier = Modifier.fillMaxSize(),
            shape = RoundedCornerShape(0.dp),
            onClick = {  }
        ) {
            Row(
                modifier = Modifier.fillMaxSize(),
                verticalAlignment = Alignment.CenterVertically,
            ) {
                FriendAvatar(
                    modifier = Modifier
                        .size(70.dp)
                        .padding(0.dp),
                    model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendAvatarViewModel()),
                    userId = friend.id
                )
                Spacer(modifier = Modifier.width(20.dp))
                ProfileUserColumn(
                    modifier = Modifier.fillMaxWidth().weight(1f),
                    name = friend.name,
                    uniqueName = friend.uniqueName,
                    userId = friend.id,
                    lastOnlineTime = null,
                )

                LaunchedEffect(Unit) {
                    model.check(friend.id)
                }
                val friendRequestRowState = model.friendRequestRowState.value

                when(friendRequestRowState) {
                    FriendRequestRowState.FRIENDS -> {
                        Text(
                            text = "Уже в друзьях",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                    FriendRequestRowState.REQUEST_SENT -> {
                        Text(
                            text = "Отправлен",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                    FriendRequestRowState.REQUEST_RECEIVED -> {
                        Text(
                            text = "Прими запрос",
                            style = MaterialTheme.typography.bodySmall,
                            color = MaterialTheme.colorScheme.primary
                        )
                    }
                    FriendRequestRowState.NOT_FRIENDS -> {
                        IconButton(
                            onClick = {
                                model.friendRequestRowState.value = FriendRequestRowState.REQUEST_SENT
                                model.addFriend(friend)
                            }
                        ) {
                            Icon(
                                painter = painterResource(id = R.drawable.person_add),
                                contentDescription = "Добавить в друзья"
                            )
                        }
                    }
                    null -> {}
                }
            }
        }
    }
}