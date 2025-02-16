package com.artem.tusaandroid.app.map

import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color

@Composable
fun MapTitle(modifier: Modifier, model: MapTitleViewModel) {
    Text(
        text = model.title,
        modifier = modifier,
        color = Color.White,
        style = MaterialTheme.typography.titleLarge
    )
}