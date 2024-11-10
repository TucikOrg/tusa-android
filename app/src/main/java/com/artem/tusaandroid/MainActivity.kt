package com.artem.tusaandroid

import android.content.Context
import android.content.SharedPreferences
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.compose.setContent
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Scaffold
import androidx.compose.runtime.Composable
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.AppLaunchActions
import com.artem.tusaandroid.app.action.MainActionFab
import com.artem.tusaandroid.app.MainActivityViewModel
import com.artem.tusaandroid.app.TestInfoLine
import com.artem.tusaandroid.app.action.auth.CredentialsManagerAuth
import com.artem.tusaandroid.app.action.friends.FriendsActionFab
import com.artem.tusaandroid.app.action.friends.PreviewFriendViewModel
import com.artem.tusaandroid.app.login.InputUniqueName
import com.artem.tusaandroid.app.map.PreviewMapViewModel
import com.artem.tusaandroid.app.map.TucikMap
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.cropper.CropperModal
import com.artem.tusaandroid.cropper.PreviewCropperModalViewModel
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.socket.ConnectionStatus
import com.artem.tusaandroid.socket.PreviewConnectionStatusViewModel
import com.artem.tusaandroid.theme.TusaAndroidTheme
import dagger.hilt.android.AndroidEntryPoint
import javax.inject.Inject


@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    @Inject
    lateinit var profileState: ProfileState
    @Inject
    lateinit var lastLocationState: LastLocationState

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val sharedPreferences: SharedPreferences = getSharedPreferences("basic", Context.MODE_PRIVATE)
        profileState.load(sharedPreferences, -1)
        lastLocationState.load(sharedPreferences, profileState.getUserId())

        setContent {
            TusaAndroidTheme {
                AppLaunchActions(hiltViewModel())
                TucikScaffold()
            }
        }
    }
}

@Composable
@Preview
fun PreviewTucikMap() {
    Box(modifier = Modifier.fillMaxSize()) {
        TucikMap(
            model = PreviewMapViewModel()
        )
    }
}

@Composable
fun TucikScaffold(model: MainActivityViewModel = hiltViewModel()) {
    Scaffold(modifier = Modifier.fillMaxSize() ) { innerPadding ->
        Box(modifier = Modifier.padding(innerPadding)) {
            TucikMap(
                model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMapViewModel())
            )

            TestInfoLine(
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(4.dp)
            )

            ConnectionStatus(
                modifier = Modifier
                    .align(Alignment.TopEnd)
                    .padding(4.dp),
                connectionStatusViewModel = TucikViewModel(preview = model.isPreview(), previewModel = PreviewConnectionStatusViewModel())
            )

            if (model.authenticationState.authenticated) {
                InputUniqueName(hiltViewModel())

                MainActionFab(modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .width(120.dp)
                    .height(120.dp)
                    .padding(16.dp),
                    hiltViewModel()
                )

                FriendsActionFab(
                    modifier = Modifier
                        .align(Alignment.BottomEnd)
                        .width(80.dp)
                        .height(80.dp)
                        .padding(16.dp),
                    model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendViewModel())
                )
            } else {
                // not authenticated
                CredentialsManagerAuth(
                    modifier = Modifier
                        .align(Alignment.BottomCenter)
                        .padding(16.dp),
                    model = hiltViewModel()
                )
            }

            CropperModal(model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewCropperModalViewModel()))
        }
    }
}