package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R

@Preview
@Composable
fun PreviewFriendsActionFab() {
    Box(
        modifier = Modifier
            .fillMaxSize()
            .padding(0.dp)
    ) {
        FriendsActionFab(
            modifier = Modifier
                .align(Alignment.BottomEnd)
                .width(80.dp)
                .height(80.dp)
                .padding(16.dp),
            PreviewFriendViewModel()
        )
    }
}

@Composable
fun FriendsActionFab(modifier: Modifier, model: FriendViewModel) {
    if (model.showModal.value) {
        FriendsModal(model)
    }

    FloatingActionButton(
        onClick = {
            model.showModal.value = true
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.group), contentDescription = "Друзья")
    }
}