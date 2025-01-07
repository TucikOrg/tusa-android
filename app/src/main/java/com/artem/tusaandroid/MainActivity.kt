package com.artem.tusaandroid

import android.content.Context
import android.content.SharedPreferences
import android.os.Bundle
import android.view.WindowManager
import androidx.activity.ComponentActivity
import androidx.activity.SystemBarStyle
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeContentPadding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.safeDrawingPadding
import androidx.compose.foundation.layout.safeGesturesPadding
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.foundation.layout.windowInsetsTopHeight
import androidx.compose.material3.Scaffold
import androidx.compose.material3.Snackbar
import androidx.compose.material3.SnackbarHost
import androidx.compose.material3.SnackbarHostState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.SideEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.view.WindowCompat
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.AppLaunchActions
import com.artem.tusaandroid.app.action.MainActionFab
import com.artem.tusaandroid.app.MainActivityViewModel
import com.artem.tusaandroid.app.TestInfoLine
import com.artem.tusaandroid.app.action.AdminFab
import com.artem.tusaandroid.app.action.auth.CredentialsManagerAuth
import com.artem.tusaandroid.app.action.chats.ChatsActionFab
import com.artem.tusaandroid.app.action.chats.ChatsViewModelPreview
import com.artem.tusaandroid.app.action.friends.FriendsActionFab
import com.artem.tusaandroid.app.action.friends.PreviewFriendViewModel
import com.artem.tusaandroid.app.chat.ChatModal
import com.artem.tusaandroid.app.dialog.AppDialog
import com.artem.tusaandroid.app.dialog.AppDialogViewModelPreview
import com.artem.tusaandroid.app.login.InputUniqueName
import com.artem.tusaandroid.app.map.PreviewMapViewModel
import com.artem.tusaandroid.app.map.TucikMap
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.selected.SelectedMarkerModal
import com.artem.tusaandroid.app.selected.SelectedMarkerViewModelPreview
import com.artem.tusaandroid.cropper.CropperModal
import com.artem.tusaandroid.cropper.PreviewCropperModalViewModel
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.MoveToMyLocationFab
import com.artem.tusaandroid.location.PreviewMoveToMyLocationViewModel
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

        enableEdgeToEdge(
            statusBarStyle = SystemBarStyle.dark(Color.Transparent.toArgb()),
            navigationBarStyle = SystemBarStyle.dark(Color.Transparent.toArgb())
        )

        setContent {
            TusaAndroidTheme() {
                AppLaunchActions(hiltViewModel())
                Tucik()
            }
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        NativeLibrary.cleanup()
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
fun Tucik(model: MainActivityViewModel = hiltViewModel()) {
    val snackbarHostState = remember { SnackbarHostState() }
    Scaffold(
        snackbarHost = {
            SnackbarHost(
                hostState = snackbarHostState,
            )
        },
    ) { it ->

        TucikMap(
            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMapViewModel())
        )

        Box(
            modifier = Modifier
                .fillMaxSize()
                .padding(it)
        ) {
            TestInfoLine(
                modifier = Modifier
                    .align(Alignment.TopCenter)
                    .padding(4.dp),
                line = "v01.2025"
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

                if (model.profileState.getUserId() == 1L) {
                    AdminFab(modifier = Modifier
                        .align(Alignment.BottomStart)
                        .padding(16.dp),
                        hiltViewModel()
                    )
                }

                Column(
                    modifier = Modifier
                        .align(Alignment.BottomEnd)
                        .padding(16.dp)
                ) {
                    MoveToMyLocationFab(
                        modifier = Modifier,
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMoveToMyLocationViewModel())
                    )
                    Spacer(modifier = Modifier.height(10.dp))
                    FriendsActionFab(
                        modifier = Modifier,
                        model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewFriendViewModel())
                    )
                    Spacer(modifier = Modifier.height(10.dp))
                    ChatsActionFab(
                        modifier = Modifier,
                        model = TucikViewModel(preview = model.isPreview(), previewModel = ChatsViewModelPreview())
                    )
                }

                SelectedMarkerModal(
                    model = TucikViewModel(preview = model.isPreview(), previewModel = SelectedMarkerViewModelPreview())
                )

                ChatModal(
                    chatViewModel = TucikViewModel(preview = model.isPreview(), previewModel = ChatsViewModelPreview())
                )

            } else {
                // not authenticated
                CredentialsManagerAuth(
                    model = hiltViewModel()
                )
            }
        }
    }

    CropperModal(model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewCropperModalViewModel()))

    AppDialog(model = TucikViewModel(preview = model.isPreview(), previewModel = AppDialogViewModelPreview()))
}