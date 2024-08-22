package com.artem.tusaandroid.ui.app

import android.graphics.BitmapFactory
import android.net.Uri
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Image
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.material3.Button
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.rememberCoroutineScope
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clip
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.compose.ui.window.Dialog
import com.artem.tusaandroid.AppVariables
import com.artem.tusaandroid.R
import com.artem.tusaandroid.uploadAvatar
import com.canhub.cropper.CropImageView
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.withContext
import okhttp3.OkHttpClient
import okhttp3.Request

@Composable
fun Avatar() {

    var showCropDialog by remember { mutableStateOf(false) }
    val pickAvatarLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.PickVisualMedia()
    ) { uri: Uri? ->
        if (uri != null) {
            AppVariables.getInstance().avatarSelectedUri = uri
            showCropDialog = true
        }
    }
    val coroutineScope = rememberCoroutineScope()

    val avaKey = "ava${AppVariables.getInstance().getDeviceAppIdentity()}"
    val avaImage = AppVariables.getInstance().getFromMem<ImageBitmap?>(avaKey)
    var avatarImageBitmap by remember { mutableStateOf(avaImage) }

    if (avaImage == null) {
        LaunchedEffect(Unit) {
            withContext(Dispatchers.IO) {
                val url = "${AppVariables.getInstance().url}/api/v1/avatar/image?owner=${AppVariables.getInstance().getDeviceAppIdentity()}"
                val okHttpClient = OkHttpClient()
                val request = Request.Builder().url(url).build()
                okHttpClient.newCall(request).execute().use { response ->
                    if (response.isSuccessful) {
                        val byteArray = response.body!!.bytes()
                        avatarImageBitmap = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.size).asImageBitmap()
                        AppVariables.getInstance().saveToMem(avaKey, avatarImageBitmap!!)
                    }
                }
            }
        }
    }


    if (showCropDialog) {
        Dialog(onDismissRequest = { showCropDialog = false }) {
            var cropImageView: CropImageView? = null
            Column {
                AndroidView(
                    factory = {
                        val cropView = CropImageView(it)
                        cropView.setAspectRatio(1, 1)
                        cropView.setImageUriAsync(AppVariables.getInstance().avatarSelectedUri)
                        cropImageView = cropView
                        return@AndroidView cropView
                    }
                )
                Button(
                    modifier = Modifier
                        .align(Alignment.CenterHorizontally)
                        .padding(horizontal = 16.dp),
                    onClick = {
                        val cropped = cropImageView!!.getCroppedImage()
                        if (cropped != null) {
                            avatarImageBitmap = cropped.asImageBitmap()
                            AppVariables.getInstance().saveToMem(avaKey, avatarImageBitmap!!)
                            uploadAvatar(coroutineScope, cropped)
                        }
                        showCropDialog = false
                    }
                ) {
                    Text("Готово")
                }
            }
        }
    }
    FloatingActionButton(
        onClick = {
            pickAvatarLauncher.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
        },
        modifier = Modifier
            .size(70.dp)
            .clip(CircleShape)
            .padding(0.dp)
    ) {
        if (avatarImageBitmap == null) {
            Icon(
                imageVector = ImageVector.vectorResource(id = R.drawable.image_52dp),
                modifier = Modifier.height(40.dp),
                contentDescription = "User avatar"
            )
        } else {
            Image(
                bitmap = avatarImageBitmap!!,
                modifier = Modifier.fillMaxSize(),
                contentDescription = "User avatar",

                )
        }
    }
}