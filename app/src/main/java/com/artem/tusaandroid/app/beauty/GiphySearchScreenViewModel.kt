package com.artem.tusaandroid.app.beauty

import android.content.Context
import android.graphics.Bitmap
import android.util.Log
import androidx.compose.runtime.MutableState
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.dto.LoginResponseDto
import com.artem.tusaandroid.dto.messenger.ChatResponse
import com.squareup.moshi.Moshi
import com.squareup.moshi.adapter
import dagger.hilt.android.lifecycle.HiltViewModel
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.flow.SharingStarted
import kotlinx.coroutines.flow.StateFlow
import kotlinx.coroutines.flow.stateIn
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request
import javax.inject.Inject


@OptIn(ExperimentalStdlibApi::class)
@HiltViewModel
open class GiphySearchScreenViewModel @Inject constructor(
    private val moshi: Moshi,
    private val client: OkHttpClient,
    private val recentlyUsedGifsDao: RecentlyUsedGifsDao
): ViewModel() {
    val gifUrls: MutableState<List<String>> = mutableStateOf(emptyList())
    var currentSearch: MutableState<String> = mutableStateOf("")
    var isLoading: MutableState<Boolean> = mutableStateOf(false)
    var showRecentlyUsedGif: MutableState<Boolean> = mutableStateOf(false)
    var isEnd: Boolean = false

    private val limit: Int = 10
    private val apiKey = "CwLAPHybfEttHLoYRzr9nDwz1dtmkd4m"
    private val searchUrl = "https://api.giphy.com/v1/gifs/search"
    private val trendingUrl = "https://api.giphy.com/v1/gifs/trending"

    val recentlyUsedGifs: StateFlow<List<RecentlyUsedGif>> = recentlyUsedGifsDao.getAllFlow()
        .stateIn(
            scope = viewModelScope,
            started = SharingStarted.WhileSubscribed(5000),
            initialValue = emptyList()
        )

    init {
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                val request = Request.Builder()
                    .url("$trendingUrl?api_key=$apiKey&limit=$limit")
                    .build()
                val response = client.newCall(request).execute()
                val json = response.body!!.string()
                val loginResponse = moshi.adapter<GiphyResponse>().fromJson(json)!!

                val urls = loginResponse.data.map { it.images.original.url }
                gifUrls.value = urls
            }
        }
    }

    @OptIn(ExperimentalStdlibApi::class)
    fun onSearch(search: String) {
        showRecentlyUsedGif.value = false
        isEnd = false
        currentSearch.value = search

        var useUrl = trendingUrl
        if (currentSearch.value.isNotEmpty()) {
            useUrl = searchUrl
        }

        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                val request = Request.Builder()
                    .url("$useUrl?api_key=$apiKey&q=$search&limit=$limit")
                    .build()
                val response = client.newCall(request).execute()
                val json = response.body!!.string()
                val loginResponse = moshi.adapter<GiphyResponse>().fromJson(json)!!

                val urls = loginResponse.data.map { it.images.original.url }
                gifUrls.value = urls
            }
        }
    }

    fun loadMoreGifs() {
        if (isEnd) {
            return
        }

        isLoading.value = true
        var useUrl = trendingUrl
        if (currentSearch.value.isNotEmpty()) {
            useUrl = searchUrl
        }
        viewModelScope.launch {
            withContext(Dispatchers.IO) {
                val offset = (gifUrls.value.size / 10).toInt() * 10
                val request = Request.Builder()
                    .url("$useUrl?api_key=$apiKey&q=${currentSearch.value}&limit=$limit&offset=$offset")
                    .build()
                val response = client.newCall(request).execute()
                val json = response.body!!.string()
                val loginResponse = moshi.adapter<GiphyResponse>().fromJson(json)!!

                val downloadedUrls = loginResponse.data.map { it.images.original.url }
                if (downloadedUrls.isEmpty()) {
                    isEnd = true
                    return@withContext
                }
                gifUrls.value = gifUrls.value.plus(downloadedUrls)
                isLoading.value = false
            }
        }
    }

    fun showRecentUsedGifs() {
        showRecentlyUsedGif.value = true
    }
}