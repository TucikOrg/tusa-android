package com.artem.tusaandroid.app.avatar

import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.animation.core.LinearEasing
import androidx.compose.animation.core.RepeatMode
import androidx.compose.animation.core.animateFloat
import androidx.compose.animation.core.infiniteRepeatable
import androidx.compose.animation.core.rememberInfiniteTransition
import androidx.compose.animation.core.tween
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.runtime.Composable
import androidx.compose.ui.platform.LocalContext
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.FloatingActionButtonDefaults
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R
import androidx.compose.material3.Icon
import androidx.compose.ui.Alignment
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.tooling.preview.Preview
import com.artem.tusaandroid.app.beauty.ShimmerBox
import com.artem.tusaandroid.socket.isClosed

@Preview
@Composable
fun MeAvatarPreview() {
    return Box(
        modifier = Modifier.fillMaxSize()
    ) {
        MeAvatar(
            modifier = Modifier.align(Alignment.Center).size(100.dp),
            model = PreviewMeAvatarViewModel()
        )
    }
}

@Composable
fun MeAvatar(modifier: Modifier, model: MeAvatarViewModel) {
    val context = LocalContext.current
    val pickAvatarLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.PickVisualMedia()
    ) { uri: Uri? ->
        if (uri == null)
            return@rememberLauncherForActivityResult

        model.avatarUriSelected(uri, context)
    }

    val avatarLoading = model.isAvatarLoading().value
    FloatingActionButton(
        onClick = {
            val state = model.connectionState?.getState()
            if (state?.value?.isClosed() == true) {
                model.appDialogState?.open("Нету интернета", "Нет смысла менять аватарку без интернета")
                return@FloatingActionButton
            }

            if (avatarLoading) {
                model.appDialogState?.open("Загружаем аватарку", "Если нету интернет соединения, то нету смысла менять аватар")
                return@FloatingActionButton
            }
            pickAvatarLauncher.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
        },
        modifier = modifier,
        elevation = FloatingActionButtonDefaults.elevation(
            defaultElevation = 0.dp,
            pressedElevation = 0.dp
        ),
    ) {

        val avatarBitmap = model.getAvatarBitmap()?.value
        if (avatarBitmap == null) {
            if (!avatarLoading) {
                Icon(
                    imageVector = ImageVector.vectorResource(id = R.drawable.image_52dp),
                    modifier = Modifier.size(40.dp),
                    contentDescription = "User avatar"
                )
            } else ShimmerBox(modifier = Modifier.fillMaxSize())
        } else {
            Image(
                bitmap = avatarBitmap.asImageBitmap(),
                modifier = Modifier.fillMaxSize(),
                contentScale = ContentScale.Crop,
                contentDescription = "User avatar",
            )
        }
    }
}



