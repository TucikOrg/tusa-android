package com.artem.tusaandroid

import android.content.Context
import android.content.SharedPreferences
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.net.Uri
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.rememberLauncherForActivityResult
import androidx.activity.compose.setContent
import androidx.activity.result.PickVisualMediaRequest
import androidx.activity.result.contract.ActivityResultContracts
import androidx.compose.foundation.Image
import androidx.compose.foundation.background
import androidx.compose.foundation.border
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.PaddingValues
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBars
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.shape.CircleShape
import androidx.compose.foundation.shape.RoundedCornerShape
import androidx.compose.material.icons.Icons
import androidx.compose.material.icons.filled.LocationOn
import androidx.compose.material.icons.filled.Person
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.FloatingActionButton
import androidx.compose.material3.Icon
import androidx.compose.material3.LinearProgressIndicator
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Text
import androidx.compose.material3.TextButton
import androidx.compose.material3.rememberModalBottomSheetState
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
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.ImageBitmap
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.vector.ImageVector
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.res.vectorResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.compose.ui.viewinterop.AndroidView
import androidx.compose.ui.window.Dialog
import com.artem.tusaandroid.ui.app.Avatar
import com.artem.tusaandroid.ui.app.MeCard
import com.artem.tusaandroid.ui.theme.TusaAndroidTheme
import com.canhub.cropper.CropImageView
import com.google.accompanist.permissions.ExperimentalPermissionsApi
import com.google.accompanist.permissions.PermissionState
import com.google.accompanist.permissions.isGranted
import com.google.accompanist.permissions.rememberPermissionState
import com.google.android.gms.location.LocationServices
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
        AppVariables.getInstance().loadFromSharedPreferences(sharedPreferences)

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
                    }
                }
            }
        }
    }
}

@OptIn(ExperimentalMaterial3Api::class, ExperimentalPermissionsApi::class)
@Composable
fun ProfileFab(modifier: Modifier) {
    var showModal by remember { mutableStateOf(false) }

    if (showModal) {
        ModalBottomSheet(
            onDismissRequest = { showModal = false }
        ) {


            val insets = WindowInsets.navigationBars.getBottom(LocalDensity.current)
            MeCard()
            LocationPermissionCard()
            Spacer(modifier = Modifier.height(insets.dp))
        }
    }

    FloatingActionButton(
        onClick = {
            showModal = true
        },
        modifier = modifier
    ) {
        Icon(imageVector = Icons.Default.Person, contentDescription = "avatar upload")
    }
}

@OptIn(ExperimentalPermissionsApi::class)
@Composable
fun LocationPermissionCard() {
    val coroutineScope = rememberCoroutineScope()
    val permissionState = rememberPermissionState(permission = android.Manifest.permission.ACCESS_FINE_LOCATION)
    if (permissionState.status.isGranted) {
        return
    }

    Column(
        modifier = Modifier
            .padding(horizontal = 16.dp, vertical = 6.dp)
            .fillMaxWidth()
            .background(Color.White)
            .border(1.dp, Color.LightGray, RoundedCornerShape(8.dp))
            .padding(8.dp),
        verticalArrangement = Arrangement.Center
    ) {
        TextButton(
            onClick = {
                coroutineScope.launch {
                    permissionState.launchPermissionRequest()
                }
            }
        ) {
            Text(
                text = "Найти меня на карте",
                fontWeight = FontWeight.Bold,
                fontSize = MaterialTheme.typography.headlineSmall.fontSize
            )
            Spacer(modifier = Modifier.width(8.dp))
            Icon(
                imageVector = Icons.Default.LocationOn,
                contentDescription = "search me on map",
                modifier = Modifier.size(30.dp)
            )
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

@OptIn(ExperimentalMaterial3Api::class, ExperimentalPermissionsApi::class)
@Composable
fun MainActionFab(modifier: Modifier) {
    var avatarImageBitmap by remember { mutableStateOf<ImageBitmap?>(null) }

    LaunchedEffect(Unit) {
        withContext(Dispatchers.IO) {
            val url = "${AppVariables.getInstance().url}/avatar/image?owner=${AppVariables.getInstance().getDeviceAppIdentity()}"
            val okHttpClient = OkHttpClient()
            val request = Request.Builder().url(url).build()
            okHttpClient.newCall(request).execute().use { response ->
                if (response.isSuccessful) {
                    val byteArray = response.body!!.bytes()
                    avatarImageBitmap = BitmapFactory.decodeByteArray(byteArray, 0, byteArray.size).asImageBitmap()
                }
            }
        }
    }

    val coroutineScope = rememberCoroutineScope()
    var showModal by remember { mutableStateOf(false) }
    val context = LocalContext.current
    val fusedLocationClient = remember { LocationServices.getFusedLocationProviderClient(context) }
    val permissionState = rememberPermissionState(permission = android.Manifest.permission.ACCESS_FINE_LOCATION)

    if (showModal) {
        ModalBottomSheet(
            onDismissRequest = { showModal = false }
        ) {
            var cropImageView: CropImageView? = null
            Button(
                modifier = Modifier
                    .align(Alignment.End)
                    .padding(horizontal = 16.dp),
                onClick = {
                    val cropped = cropImageView!!.getCroppedImage()
                    if (cropped != null) {
                        avatarImageBitmap = cropped.asImageBitmap()
                        uploadAvatar(coroutineScope, cropped)
                        if (!permissionState.status.isGranted) {
                            permissionState.launchPermissionRequest()
                        }
                    }
                    showModal = false
                }
            ) {
                Text("Дальше")
            }
            AndroidView(
                factory = {
                    val cropView = CropImageView(it)
                    cropView.setAspectRatio(1, 1)
                    cropView.setImageUriAsync(AppVariables.getInstance().avatarSelectedUri)
                    cropImageView = cropView
                    return@AndroidView cropView
                }
            )
        }
        return
    }

    val pickAvatarLauncher = rememberLauncherForActivityResult(
        contract = ActivityResultContracts.PickVisualMedia()
    ) { uri: Uri? ->
        if (uri != null) {
            AppVariables.getInstance().avatarSelectedUri = uri
            showModal = true
        }
    }

    FloatingActionButton(
        onClick = {
            pickAvatarLauncher.launch(PickVisualMediaRequest(ActivityResultContracts.PickVisualMedia.ImageOnly))
        },
        shape = RoundedCornerShape(60.dp),
        modifier = modifier
    ) {
        if (avatarImageBitmap != null) {
            Image(
                bitmap = avatarImageBitmap!!,
                contentDescription = "avatar"
            )
        } else {
            Icon(imageVector = Icons.Default.Person, contentDescription = "avatar upload")
        }
    }
}


@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun ModalBottomSheetExample() {
    val sheetState = rememberModalBottomSheetState(
        skipPartiallyExpanded = true
    )
    val coroutineScope = rememberCoroutineScope()
    var showModal by remember { mutableStateOf(false) }

    // Main content with button to show the modal
    Column(
        modifier = Modifier
            .fillMaxSize()
            .padding(16.dp),
        verticalArrangement = Arrangement.Center,
        horizontalAlignment = Alignment.CenterHorizontally
    ) {
        Button(onClick = { showModal = true }) {
            Text("Show Modal Bottom Sheet")
        }
    }

    if (showModal) {
        ModalBottomSheet(
            onDismissRequest = { showModal = false },
            sheetState = sheetState
        ) {
            // Content inside the modal
        }
    }
}

fun uploadAvatar(coroutineScope: CoroutineScope, cropped: Bitmap) {
    coroutineScope.launch {
        withContext(Dispatchers.IO) {
            val uploadUrl = "${AppVariables.getInstance().url}/avatar"
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

@Composable
fun Greeting(name: String, modifier: Modifier = Modifier) {
    Text(
        text = "Hello фывфыв $name!",
        modifier = modifier
    )
}

@Preview(showBackground = true)
@Composable
fun GreetingPreview() {
    TusaAndroidTheme {
        Greeting("Android")
    }
}