package com.artem.tusaandroid.app

import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.Person
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.ui.Modifier
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.R
import com.artem.tusaandroid.room.FriendViewModel

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun FriendsFab(modifier: Modifier, friendViewModel: FriendViewModel = hiltViewModel()) {
    val showModal = remember { mutableStateOf(false) }

    if (showModal.value) {
        ModalBottomSheet(
            onDismissRequest = { showModal.value = false },
        ) {
            val friends by friendViewModel.allFriends.collectAsState()
            Column(
                modifier = Modifier.padding(horizontal = 10.dp)
            ) {
                friends.forEach {
                    Text(text = it.installAppId)
                }
                Spacer(modifier = Modifier.height(40.dp))
            }
        }
    }

    FloatingActionButton(
        onClick = {
            showModal.value = true
        },
        modifier = modifier
    ) {
        Icon(painter = painterResource(id = R.drawable.love_friends), contentDescription = "My friends")
    }
}