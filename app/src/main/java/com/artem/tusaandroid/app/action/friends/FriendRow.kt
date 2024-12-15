package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.ElevatedButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.isPreview

@Composable
@Preview
fun PreviewFriendRow() {
    FriendRow(
        model = PreviewFriendRowViewModel(),
        friend = FriendDto(
            id = 1,
            name = "Name",
            uniqueName = "ivanov"
        )
    )
}

@Composable
fun FriendRow(
    model: FriendRowViewModel,
    friend: FriendDto
) {
    val scope = rememberCoroutineScope()
    val leftSwipeFriendRow = LeftSwipeFriendRow(scope, R.drawable.person_remove)

    LaunchedEffect(Unit) {
        leftSwipeFriendRow.onLeftSwiped = {
            model.removeFriend(friend.id!!)
        }
    }
    leftSwipeFriendRow.SwipeWrapper {
        FriendRowCard(
            model = model,
            friend = friend
        )
    }
}

@Composable
fun FriendRowCard(
    model: FriendRowViewModel,
    friend: FriendDto
) {
    // Карточка основного контента. Пользователь какой-то
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
        }
    }
}