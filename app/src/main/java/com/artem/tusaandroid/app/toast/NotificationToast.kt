package com.artem.tusaandroid.app.toast

import androidx.compose.animation.core.*
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.*
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.Button
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.*
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import androidx.hilt.navigation.compose.hiltViewModel
import coil.compose.AsyncImage
import com.artem.tusaandroid.app.action.friends.FriendAvatar
import kotlinx.coroutines.delay

data class ToastItem(
    val id: Int,
    val avatarId: Long,
    val title: String,    // Заголовок
    val message: String   // Текст
)

@Composable
fun NotificationToast(
    toastItem: ToastItem,
    onDismiss: () -> Unit,
    position: Int
) {
    var isVisible by remember { mutableStateOf(false) }

    val offsetX by animateFloatAsState(
        targetValue = if (isVisible) 0f else 300f,
        animationSpec = tween(durationMillis = 300, easing = FastOutSlowInEasing),
        label = "toastXAnimation"
    )

    val offsetY by animateFloatAsState(
        targetValue = (16 + position * 80).toFloat(), // Увеличила высоту до 80.dp для нового layout
        animationSpec = tween(durationMillis = 300, easing = FastOutSlowInEasing),
        label = "toastYAnimation"
    )

    LaunchedEffect(Unit) {
        isVisible = true
        delay(3000L)
        isVisible = false
        delay(300)
        onDismiss()
    }

    if (offsetX < 300f || isVisible) {
        Box(
            modifier = Modifier
                .fillMaxSize()
                .padding(end = 16.dp),
            contentAlignment = Alignment.TopEnd
        ) {
            Box(
                modifier = Modifier
                    .offset(x = offsetX.dp, y = offsetY.dp)
                    .width(300.dp) // Увеличила ширину для картинки и текста
                    .background(
                        color = MaterialTheme.colorScheme.background,
                        shape = RoundedCornerShape(8.dp)
                    )
                    .padding(12.dp)
            ) {
                Row(
                    verticalAlignment = Alignment.CenterVertically,
                    modifier = Modifier.fillMaxWidth()
                ) {
                    FriendAvatar(
                        modifier = Modifier
                            .size(48.dp)
                            .clip(CircleShape),
                        model = hiltViewModel(),
                        userId = toastItem.avatarId,
                        contentScale = ContentScale.Crop
                    )

                    // Заголовок и текст
                    Column(
                        modifier = Modifier
                            .padding(start = 12.dp)
                            .weight(1f)
                    ) {
                        Text(
                            text = toastItem.title,
                            fontSize = 16.sp,
                            fontWeight = androidx.compose.ui.text.font.FontWeight.Bold
                        )
                        Text(
                            text = toastItem.message,
                            fontSize = 14.sp,
                            modifier = Modifier.padding(top = 4.dp)
                        )
                    }
                }
            }
        }
    }
}

@Composable
fun Toasts(model: ToastsViewModel = hiltViewModel()) {
    var toasts = model.toastsState.toasts

    toasts.forEachIndexed { index, toastItem ->
        key(toastItem.id) {
            NotificationToast(
                toastItem = toastItem,
                onDismiss = { toasts.remove(toastItem) },
                position = index
            )
        }
    }
}