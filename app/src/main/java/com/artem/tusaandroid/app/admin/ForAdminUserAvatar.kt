package com.artem.tusaandroid.app.admin

import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.FloatingActionButtonDefaults
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R

@Composable
fun ForAdminUserAvatar(modifier: Modifier, model: ForAdminUserAvatarViewModel, userId: Long) {
    val context = LocalContext.current
    val pickAvatarLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.PickVisualMedia()
    ) { uri: Uri? ->
        if (uri == null)
            return@rememberLauncherForActivityResult

        model.avatarUriSelected(uri, context, userId)
    }

    LaunchedEffect(Unit) {
        model.loadAvatar(userId)
    }

    FloatingActionButton(
        onClick = {
            pickAvatarLauncher.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
        },
        modifier = modifier,
        elevation = FloatingActionButtonDefaults.elevation(
            defaultElevation = 0.dp,
            pressedElevation = 0.dp
        )
    ) {
        val avatarBitmap = model.getAvatarBitmap(userId)?.value
        if (avatarBitmap == null) {
            Icon(
                imageVector = ImageVector.vectorResource(id = R.drawable.image_52dp),
                modifier = Modifier.height(40.dp),
                contentDescription = "User avatar"
            )
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