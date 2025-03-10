package com.artem.tusaandroid.app.action.friends

import androidx.compose.material3.BadgedBox
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import com.artem.tusaandroid.R
import com.artem.tusaandroid.app.beauty.NewMessagesBadge

@Composable
fun FriendsActionFab(modifier: Modifier, model: FriendViewModel) {
    if (model.showModal.value) {
        FriendsModal(model)
    }

    val newFriendsRequests by model.getNewFriendsRequestsCount().collectAsState()
    BadgedBox(
        badge = {
            NewMessagesBadge(
                unreadMessages = newFriendsRequests,
                modifier = Modifier
            )
        }
    ) {
        FloatingActionButton(
            onClick = {
                model.showModal.value = true
            },
            modifier = modifier
        ) {
            Icon(painter = painterResource(id = R.drawable.group), contentDescription = "Друзья")
        }
    }
}