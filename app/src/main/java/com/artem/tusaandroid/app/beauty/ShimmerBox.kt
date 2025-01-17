package com.artem.tusaandroid.app.beauty

import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.size
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@Composable
@Preview
fun PreviewShimmerBox() {
    ShimmerBox(
        modifier = Modifier.size(40.dp)
    )
}

@Composable
fun ShimmerBox(modifier: Modifier, shimmerWidth: Float = 200.0f, len: Float = 500.0f) {
    val infiniteTransition = rememberInfiniteTransition(label = "")
    val shimmerTranslate = infiniteTransition.animateFloat(
        initialValue = 0f,
        targetValue = 1f,
        animationSpec = infiniteRepeatable(
            animation = tween(durationMillis = 900, easing = LinearEasing),
            repeatMode = RepeatMode.Restart
        ),
        label = ""
    )

    val from = -shimmerWidth * 2.0f
    val to = from + shimmerWidth
    val moveValue = shimmerTranslate.value * len
    val shimmerBrush = Brush.linearGradient(
        colors = listOf(
            Color.LightGray.copy(alpha = 0.5f),
            Color.Gray.copy(alpha = 0.3f),
            Color.LightGray.copy(alpha = 0.5f)
        ),
        start = Offset(x = to + moveValue, y = 0f),
        end = Offset(x = from + moveValue, y = 200f)
    )

    Box(
        modifier = modifier
            .background(brush = shimmerBrush)
    )
}