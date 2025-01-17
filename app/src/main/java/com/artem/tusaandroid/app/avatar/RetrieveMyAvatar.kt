package com.artem.tusaandroid.app.avatar

import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect

@Composable
fun RetrieveMyAvatar(
    model: LoadAvatarViewModel
) {
    LaunchedEffect(Unit) {
        model.retrieveMyAvatarAndUpdateMarker()
    }
}