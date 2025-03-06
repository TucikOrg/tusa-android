package com.artem.tusaandroid.app.beauty

import android.content.Context
import androidx.compose.foundation.clickable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.lazy.grid.GridCells
import androidx.compose.foundation.lazy.grid.LazyVerticalGrid
import androidx.compose.foundation.lazy.grid.rememberLazyGridState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.collectAsState
import androidx.compose.runtime.getValue
import androidx.compose.runtime.snapshotFlow
import androidx.compose.ui.Modifier
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.lifecycle.compose.LocalLifecycleOwner
import com.artem.tusaandroid.app.chat.ChatViewModel

@Composable
fun GiphyList(
    giphySearchScreen: GiphySearchScreenViewModel,
    chatViewModel: ChatViewModel,
    onLoadMore: () -> Unit
) {
    val gifUrlsState: MutableState<List<String>> = giphySearchScreen.gifUrls
    val gridState = rememberLazyGridState()

    if (giphySearchScreen.showRecentlyUsedGif.value) {
        val recentlyUsedGifs by giphySearchScreen.recentlyUsedGifs.collectAsState()
        LazyVerticalGrid(
            columns = GridCells.Fixed(2), // 2 столбца
            modifier = Modifier.fillMaxWidth(),
            state = gridState,
            verticalArrangement = Arrangement.spacedBy(0.dp),
            horizontalArrangement = Arrangement.spacedBy(0.dp)
        ) {
            items(recentlyUsedGifs.size) { gifUrlIndex ->
                val gifUrl = recentlyUsedGifs[gifUrlIndex].url
                GifInList(chatViewModel, gifUrl)
            }
        }
    } else {
        val gifUrls = gifUrlsState.value
        LazyVerticalGrid(
            columns = GridCells.Fixed(2), // 2 столбца
            modifier = Modifier.fillMaxWidth(),
            state = gridState,
            verticalArrangement = Arrangement.spacedBy(0.dp),
            horizontalArrangement = Arrangement.spacedBy(0.dp)
        ) {
            items(gifUrls.size) { gifUrlIndex ->
                val gifUrl = gifUrls[gifUrlIndex]
                GifInList(chatViewModel, gifUrl)
            }
        }
    }



    // Отслеживание конца списка
    LaunchedEffect(gridState) {
        snapshotFlow { gridState.layoutInfo }
            .collect { layoutInfo ->
                val totalItems = layoutInfo.totalItemsCount
                val lastVisibleItem = layoutInfo.visibleItemsInfo.lastOrNull()?.index ?: 0
                if (giphySearchScreen.isLoading.value == false && totalItems > 0 && lastVisibleItem >= totalItems - 2) { // Порог: 2 элемента до конца
                    onLoadMore() // Вызываем загрузку следующей страницы
                }
            }
    }
}

@Composable
fun GifInList(chatViewModel: ChatViewModel, gifUrl: String) {
    val context = LocalContext.current
    val lifecycleOwner = LocalLifecycleOwner.current
    GifWrapper(
        modifier = Modifier
            .size(150.dp) // Уменьшите размер для сетки
            .clickable { chatViewModel.onGifClick(gifUrl, context, lifecycleOwner) },
        url = gifUrl
    )
}