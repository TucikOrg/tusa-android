package com.artem.tusaandroid.app.avatar

import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.graphics.Matrix
import android.media.ExifInterface
import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
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
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.layout.ContentScale

@Composable
fun MeAvatar(modifier: Modifier, model: MeAvatarViewModel) {
    val context = LocalContext.current
    val pickAvatarLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.PickVisualMedia()
    ) { uri: Uri? ->
        if (uri == null)
            return@rememberLauncherForActivityResult

        var matrix: Matrix?
        var bitmap: Bitmap?

        context.contentResolver.openInputStream(uri).use { inputStream ->
            val exif = ExifInterface(inputStream!!)
            val orientation = exif.getAttributeInt(ExifInterface.TAG_ORIENTATION, ExifInterface.ORIENTATION_NORMAL)
            val rotationDegrees = when (orientation) {
                ExifInterface.ORIENTATION_ROTATE_90 -> 90
                ExifInterface.ORIENTATION_ROTATE_180 -> 180
                ExifInterface.ORIENTATION_ROTATE_270 -> 270
                else -> 0
            }
            matrix = Matrix().apply { postRotate(rotationDegrees.toFloat()) }
        }

        context.contentResolver.openInputStream(uri).use { inputStream ->
            val originalBitmap = BitmapFactory.decodeStream(inputStream)
            bitmap = Bitmap.createBitmap(originalBitmap, 0, 0, originalBitmap.width, originalBitmap.height, matrix, true)
        }

        model.saveAvatarBitmap(bitmap!!, context)
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
        val avatarBitmap = model.getAvatarBitmap().value
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