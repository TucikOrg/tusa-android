package com.artem.tusaandroid.app.chat

import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.StartOffset
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.runtime.Composable
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.graphicsLayer
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp

@Composable
@Preview
fun TypingIndicator(modifier: Modifier = Modifier) {
    val dotCount = 3
    val infiniteTransition = rememberInfiniteTransition(label = "")

    val animations = List(dotCount) { index ->
        infiniteTransition.animateFloat(
            initialValue = 0f,
            targetValue = 1f,
            animationSpec = infiniteRepeatable(
                animation = tween(durationMillis = 600, easing = LinearEasing),
                repeatMode = RepeatMode.Restart,
                initialStartOffset = StartOffset(index * 200)
            ), label = ""
        )
    }

    Row(modifier, horizontalArrangement = Arrangement.spacedBy(4.dp)) {
        animations.forEach { animation ->
            Box(
                Modifier
                    .size(8.dp)
                    .graphicsLayer { alpha = animation.value }
                    .background(Color.Gray, CircleShape)
            )
        }
    }
}