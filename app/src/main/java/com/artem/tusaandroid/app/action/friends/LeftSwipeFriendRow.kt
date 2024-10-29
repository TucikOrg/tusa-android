package com.artem.tusaandroid.app.action.friends

import androidx.compose.animation.core.Animatable
import androidx.compose.animation.core.EaseIn
import androidx.compose.animation.core.tween
import androidx.compose.foundation.background
import androidx.compose.foundation.gestures.DraggableState
import androidx.compose.foundation.gestures.Orientation
import androidx.compose.foundation.gestures.draggable
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.fillMaxHeight
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.offset
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.unit.Density
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.IntOffset
import androidx.compose.ui.unit.dp
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.launch
import kotlin.math.abs

class LeftSwipeFriendRow(
    viewModelScope: CoroutineScope,
    private val useIconRes: Int
) {
    var onLeftSwiped: (() -> Unit)? = null

    private fun getWidth(density: Density): Dp {
        return with(density) { offsetX.value.toDp() }
    }
    private fun getProgress(): Float = (offsetX.value / -actionThreshold).coerceIn(0f, 1.0f)


    private val actionThreshold = 500
    private val offsetX = Animatable(0f)
    private val draggableState = DraggableState { delta ->
        viewModelScope.launch {
            val targetValue = (offsetX.value + delta).coerceAtMost(0f).coerceIn(-actionThreshold.toFloat(), 0f)
            offsetX.snapTo(targetValue)

            if (targetValue == -actionThreshold.toFloat()) {
                offsetX.snapTo(0f)
                onLeftSwiped?.invoke()
            }
        }
    }

    private suspend fun onDragStopped() {
        val targetValue = 0.0f
        offsetX.animateTo(
            targetValue,
            animationSpec = tween(100, easing = EaseIn)
        )
    }

    @Composable
    fun SwipeWrapper(content: @Composable () -> Unit) {
        val density = LocalDensity.current
        val height = 90.dp
        val offsetValue = offsetX.value.toInt()
        val offsetValueAbs = abs(offsetValue)
        val progress = offsetValueAbs.toFloat() / actionThreshold
        val widthDp = with(density) { offsetValueAbs.toDp() }
        Box(
            modifier = Modifier
                .fillMaxWidth()
                .height(height)
        ) {
            Row(
                modifier = Modifier
                    .fillMaxHeight()
                    .background(Color.Red.copy(alpha = progress.coerceIn(0f, 1.0f) * 0.5f))
                    .width(widthDp)
                    .align(Alignment.CenterEnd),
                horizontalArrangement = Arrangement.Center,
                verticalAlignment = Alignment.CenterVertically
            ) {
                Icon(
                    imageVector = ImageVector.vectorResource(id = useIconRes),
                    contentDescription = "Delete",
                    modifier = Modifier
                        .size(50.dp)
                        .padding(10.dp),
                    tint = Color.Red
                )
            }
            Box(
                modifier = Modifier
                    .fillMaxSize()
                    .offset { IntOffset(offsetValue, 0) }
                    .draggable(
                        orientation = Orientation.Horizontal,
                        state = draggableState,
                        onDragStopped = {
                            onDragStopped()
                        }
                    )
            ) {
                content()
            }
        }
    }
}