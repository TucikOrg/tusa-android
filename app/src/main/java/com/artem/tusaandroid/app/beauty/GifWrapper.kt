package com.artem.tusaandroid.app.beauty

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.material3.MaterialTheme
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.painter.BitmapPainter
import androidx.compose.ui.graphics.painter.ColorPainter
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import coil.compose.SubcomposeAsyncImage
import coil.decode.GifDecoder
import coil.request.CachePolicy
import coil.request.ImageRequest

@Composable
fun GifWrapper(modifier: Modifier, url: String, model: GifWrapperViewModel = hiltViewModel()) {
    val context = LocalContext.current
    val colorPainter = ColorPainter(MaterialTheme.colorScheme.surface)

    SubcomposeAsyncImage(
        model = ImageRequest.Builder(context)
            .data(url)
            .decoderFactory(GifDecoder.Factory()) // Поддержка GIF-анимации
            .memoryCachePolicy(CachePolicy.ENABLED)
            .diskCachePolicy(CachePolicy.ENABLED)
            .build(),
        contentDescription = "GIF from chat",
        modifier = modifier,
        contentScale = ContentScale.Crop,
        loading = {
            // Показываем статичный первый кадр или серый фон во время загрузки
            var imageBitmap by remember { model.getGifByUrl(url, context) }
            if (imageBitmap != null) {
                Image(
                    painter = BitmapPainter(imageBitmap!!.asImageBitmap()),
                    contentDescription = null,
                    modifier = Modifier.fillMaxWidth().height(300.dp),
                    contentScale = ContentScale.Crop
                )
            } else {
                ShimmerBox(modifier = Modifier.fillMaxWidth().height(300.dp))
            }

        },
        error = {
            // В случае ошибки показываем серый фон
            Image(
                painter = colorPainter,
                contentDescription = "Error loading GIF",
                modifier = Modifier.fillMaxWidth().height(300.dp),
                contentScale = ContentScale.Crop
            )
        }
    )
}