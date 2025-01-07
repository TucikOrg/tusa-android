package com.artem.tusaandroid.app.chat

import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Brush
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@Preview
@Composable
fun ChatLoadingPreview() {
    ChatLoadingPlaceholder()
}

@Composable
fun ChatLoadingPlaceholder() {
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

    val moveValue = shimmerTranslate.value * 1200f
    val shimmerBrush = Brush.linearGradient(
        colors = listOf(
            Color.LightGray.copy(alpha = 0.5f),
            Color.Gray.copy(alpha = 0.3f),
            Color.LightGray.copy(alpha = 0.5f)
        ),
        start = Offset(x = -400f + moveValue, y = 0f),
        end = Offset(x = -200f + moveValue, y = 200f)
    )

    Column(
        modifier = Modifier
            .fillMaxWidth()
            .padding(16.dp)
    ) {
        repeat(10) { index ->
            PlaceholderBubble(
                modifier = Modifier
                    .align(if (index % 2 == 0) Alignment.Start else Alignment.End),
                shimmerBrush = shimmerBrush
            )
            Spacer(modifier = Modifier.height(8.dp))
        }
    }
}

@Composable
fun PlaceholderBubble(
    modifier: Modifier = Modifier,
    shimmerBrush: Brush
) {
    Box(
        modifier = modifier
            .size(width = 200.dp, height = 90.dp)
            .background(
                brush = shimmerBrush,
                shape = RoundedCornerShape(16.dp)
            )
    )
}