package com.artem.tusaandroid

import android.content.Context
import android.content.SharedPreferences
import android.graphics.Bitmap
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Scaffold
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import com.artem.tusaandroid.ui.app.ProfileFab
import com.artem.tusaandroid.ui.app.SettingsFab
import com.artem.tusaandroid.ui.theme.TusaAndroidTheme
import kotlinx.coroutines.CoroutineScope
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.launch
import kotlinx.coroutines.withContext
import okhttp3.MediaType.Companion.toMediaType
import okhttp3.MultipartBody
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import java.io.ByteArrayOutputStream


class MainActivity : ComponentActivity() {

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val sharedPreferences: SharedPreferences = getSharedPreferences("basic", Context.MODE_PRIVATE)
        AppVariables.getInstance().setSharedPreference(sharedPreferences)
        AppVariables.getInstance().load()

        setContent {
            TusaAndroidTheme {
                Scaffold(modifier = Modifier.fillMaxSize() ) { innerPadding ->
                    Box(modifier = Modifier.padding(innerPadding)) {
                        Map()
                        ProfileFab(modifier = Modifier
                            .align(Alignment.BottomCenter)
                            .width(120.dp)
                            .height(120.dp)
                            .padding(16.dp)
                        )
                        SettingsFab(modifier = Modifier
                            .align(Alignment.TopEnd)
                            .width(80.dp)
                            .height(80.dp)
                            .padding(top = 31.dp, end = 7.dp)
                        )
                    }
                }
            }
        }
    }
}

@Composable
fun Map(innerPadding: PaddingValues = PaddingValues(0.dp)) {
    AndroidView(
        factory = { MapView(it) },
        modifier = Modifier.padding(innerPadding)
    )
}

fun uploadAvatar(coroutineScope: CoroutineScope, cropped: Bitmap) {
    coroutineScope.launch {
        withContext(Dispatchers.IO) {
            val uploadUrl = "${AppVariables.getInstance().url}/api/v1/avatar"
            val byteArrayOutputStream = ByteArrayOutputStream()
            cropped.compress(Bitmap.CompressFormat.JPEG, 60, byteArrayOutputStream)
            val requestBody = byteArrayOutputStream.toByteArray()
                .toRequestBody("image/jpeg".toMediaType())
            val sizeInBytes: Int = byteArrayOutputStream.size()
            val sizeInMB = sizeInBytes / (1024.0 * 1024.0)
            val multipartBody = MultipartBody.Builder()
                .setType(MultipartBody.FORM)
                .addFormDataPart("owner", AppVariables.getInstance().getDeviceAppIdentity())
                .addFormDataPart(
                    "file",
                    "avatar.png",
                    requestBody
                ).build()

            val request = Request.Builder()
                .url(uploadUrl)
                .post(multipartBody)
                .build()

            val client = OkHttpClient()
            val response = client.newCall(request).execute()
            val successful = response.isSuccessful
        }
    }
}
