package com.artem.tusaandroid.app.action.friends

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
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.lerp
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.R
import com.artem.tusaandroid.TucikViewModel
import com.artem.tusaandroid.dto.FriendDto
import com.artem.tusaandroid.isPreview
import java.time.Instant
import java.time.ZoneId
import java.time.format.DateTimeFormatter
import androidx.compose.ui.graphics.lerp
import com.artem.tusaandroid.app.profile.ProfileUserColumn

@Composable
fun FriendRow(
    model: FriendRowViewModel,
    friend: FriendDto
) {
    val scope = rememberCoroutineScope()
    val leftSwipeFriendRow = LeftSwipeFriendRow(scope, R.drawable.person_remove) {
        model.removeFriend(friend.id)
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
                userId = friend.id
            )
            Spacer(modifier = Modifier.width(15.dp))
            ProfileUserColumn(
                modifier = Modifier.fillMaxWidth().weight(1f),
                name = friend.name,
                uniqueName = friend.uniqueName,
                userId = friend.id,
                lastOnlineTime = friend.lastOnlineTime
            )
            //Spacer(modifier = Modifier.weight(1f))
            IconButton(
                onClick = { model.openChat(friend) }
            ) {
                Icon(
                    imageVector = ImageVector.vectorResource(id = R.drawable.chat),
                    contentDescription = "Chat"
                )
            }
        }
    }
}