package com.artem.tusaandroid.app.map

import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.padding
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import com.artem.tusaandroid.MapView

@Composable
fun TucikMap(model: MapViewModel, innerPadding: PaddingValues = PaddingValues(0.dp)) {
    AndroidView(
        factory = { MapView(it, model.meAvatarState!!, model.lastLocationState!!) },
        modifier = Modifier.padding(innerPadding)
    )
}