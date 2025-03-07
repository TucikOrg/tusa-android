package com.artem.tusaandroid.app.beauty

import androidx.compose.material3.Badge
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier

@Composable
fun NewMessagesBadge(unreadMessages: Int, modifier: Modifier) {
    var useNumber = unreadMessages.toString()
    if (unreadMessages > 99) {
        useNumber = "99+"
    }
    if (unreadMessages > 0) {
        Badge(
            modifier = modifier,
            containerColor = MaterialTheme.colorScheme.error
        ) {
            Text(
                useNumber,
            )
        }
    }
}