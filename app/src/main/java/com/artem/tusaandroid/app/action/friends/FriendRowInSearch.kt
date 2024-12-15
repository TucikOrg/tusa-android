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
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.isPreview
import androidx.compose.runtime.getValue
import androidx.compose.runtime.setValue

@Composable
fun FriendRowInSearch(model: FriendRowInSearchViewModel, friend: FriendDto) {
//    LaunchedEffect(Unit) {
//        model.checkAlreadyFriend(friend.id!!)
//    }

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
                    userId = friend.id!!
                )
                Spacer(modifier = Modifier.width(20.dp))
                Column(
                    modifier = Modifier.weight(1.0f),
                ) {
                    Text(
                        modifier = Modifier,
                        text = friend.name,
                        style = MaterialTheme.typography.headlineSmall
                    )
                    Spacer(modifier = Modifier.height(3.dp))
                    if (friend.uniqueName != null) {
                        Text(
                            text = "@${friend.uniqueName}",
                            style = MaterialTheme.typography.bodySmall,
                            color = Color.Gray
                        )
                    }
                }


                var requestSent by remember { mutableStateOf(false) }
                if (model.checkAlreadyFriend(friend.id)) {
                    Text(
                        text = "Уже в друзьях",
                        style = MaterialTheme.typography.bodySmall,
                        color = MaterialTheme.colorScheme.primary
                    )
                } else if (requestSent == false) {
                    IconButton(
                        onClick = {
                            requestSent = true
                            model.addFriend(friend)
                        }
                    ) {
                        Icon(
                            painter = painterResource(id = R.drawable.person_add),
                            contentDescription = "Добавить в друзья"
                        )
                    }
                }
            }
        }
    }
}