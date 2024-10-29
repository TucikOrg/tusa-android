package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R

@Composable
fun FindFriendButton(model: FindFriendViewModel, friendsVM: FriendViewModel) {
    if (model.showModal.value) {
        FindFriendsModal(model, friendsVM)
    }

    TextButton(
        onClick = {
            model.showModal.value = true
        }
    ) {
        Icon(
            imageVector = ImageVector.vectorResource(id = R.drawable.search),
            contentDescription = "Find friend",
            modifier = Modifier.size(24.dp),
        )
        Spacer(modifier = Modifier.width(5.dp))
        Text("Найти друга")
    }
}