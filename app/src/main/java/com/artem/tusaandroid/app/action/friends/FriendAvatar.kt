package com.artem.tusaandroid.app.action.friends

import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.FloatingActionButtonDefaults
import androidx.compose.material3.Icon
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.layout.ContentScale
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R

@Composable
fun FriendAvatar(
    modifier: Modifier,
    model: FriendAvatarViewModel, userId: Long
) {
    LaunchedEffect(Unit) {
        model.loadAvatar(userId)
    }

    FloatingActionButton(
        onClick = { },
        modifier = modifier,
        shape = RoundedCornerShape(16.dp),
        elevation =  FloatingActionButtonDefaults.elevation(
            defaultElevation = 0.dp,
            pressedElevation = 0.dp
        )
    ) {
        val avatarBitmap = model.getState(userId)?.value
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