package com.artem.tusaandroid.app.beauty

import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxWidth
import com.artem.tusaandroid.R
import androidx.compose.material3.Icon
import androidx.compose.material3.IconButton
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalFocusManager
import androidx.compose.ui.platform.LocalSoftwareKeyboardController
import androidx.compose.ui.res.painterResource
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.chat.ChatViewModel


@Composable
fun GiphySearchScreen(
    modifier: Modifier,
    chatViewModel: ChatViewModel,
    giphySearchScreen: GiphySearchScreenViewModel = hiltViewModel()
) {
    Column(
        modifier = modifier
    ) {
        val focusManager = LocalFocusManager.current
        val keyboard = LocalSoftwareKeyboardController.current

        Row(
            modifier = Modifier
                .fillMaxWidth()
                .background(MaterialTheme.colorScheme.background),
            verticalAlignment = Alignment.CenterVertically
        ) {
            SearchBar(
                modifier = Modifier.weight(1f),
                onSearch = {
                    giphySearchScreen.onSearch(it)
                    keyboard?.hide()
                    focusManager.clearFocus()
                }
            )
            IconButton(
                onClick = {
                    giphySearchScreen.showRecentUsedGifs()
                }
            ) {
                Icon(
                    painter = painterResource(R.drawable.history),
                    contentDescription = "Gifs history"
                )
            }
        }

        GiphyList(
            giphySearchScreen = giphySearchScreen,
            chatViewModel = chatViewModel,
            onLoadMore = {
                giphySearchScreen.loadMoreGifs()
            }
        )
    }
}