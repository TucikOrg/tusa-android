package com.artem.tusaandroid.app

import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.hilt.navigation.compose.hiltViewModel


@Composable
fun LoadMeAvatar(loadMeAvatarViewModel: LoadMeAvatarViewModel = hiltViewModel()) {
    LaunchedEffect(Unit) {
        loadMeAvatarViewModel.loadMeAvatar()
    }
}