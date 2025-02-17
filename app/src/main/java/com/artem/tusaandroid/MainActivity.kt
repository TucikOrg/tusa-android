package com.artem.tusaandroid

import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.os.Bundle
import android.util.Log
import androidx.activity.ComponentActivity
import androidx.activity.SystemBarStyle
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.size
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Scaffold
import androidx.compose.material3.SnackbarHost
import androidx.compose.material3.SnackbarHostState
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.platform.LocalContext
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.hilt.navigation.compose.hiltViewModel
import androidx.work.Data
import androidx.work.OneTimeWorkRequestBuilder
import androidx.work.WorkManager
import com.artem.tusaandroid.app.action.MainActionFab
import com.artem.tusaandroid.app.MainActivityViewModel
import com.artem.tusaandroid.app.TestInfoLine
import com.artem.tusaandroid.app.action.AdminFab
import com.artem.tusaandroid.app.action.auth.CredentialsManagerAuth
import com.artem.tusaandroid.app.action.chats.ChatsActionFab
import com.artem.tusaandroid.app.action.friends.FriendsActionFab
import com.artem.tusaandroid.app.action.logs.LogsFab
import com.artem.tusaandroid.app.action.logs.LogsFabViewModelPreview
import com.artem.tusaandroid.app.avatar.RetrieveMyAvatar
import com.artem.tusaandroid.app.chat.ChatModal
import com.artem.tusaandroid.app.dialog.AppDialog
import com.artem.tusaandroid.app.dialog.AppDialogViewModelPreview
import com.artem.tusaandroid.app.login.InputUniqueName
import com.artem.tusaandroid.app.logs.CrashLogUploadWorker
import com.artem.tusaandroid.app.map.MapTitle
import com.artem.tusaandroid.app.map.PreviewMapViewModel
import com.artem.tusaandroid.app.map.TucikMap
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.selected.SelectedMarkerModal
import com.artem.tusaandroid.app.state.RefreshStateListeners
import com.artem.tusaandroid.app.state.RefreshStateListenersViewModel
import com.artem.tusaandroid.cropper.CropperModal
import com.artem.tusaandroid.cropper.PreviewCropperModalViewModel
import com.artem.tusaandroid.dto.CrashData
import com.artem.tusaandroid.firebase.FirebaseState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.ListenLocationsUpdates
import com.artem.tusaandroid.location.LoadAllFriendsLocations
import com.artem.tusaandroid.location.MoveToMyLocationFab
import com.artem.tusaandroid.location.PreviewMoveToMyLocationViewModel
import com.artem.tusaandroid.notification.NotificationsEnabledCheck
import com.artem.tusaandroid.notification.NotificationsEnabledCheckViewModelPreview
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.socket.ConnectionStatus
import com.artem.tusaandroid.socket.PreviewConnectionStatusViewModel
import com.artem.tusaandroid.socket.SocketConnect
import com.artem.tusaandroid.socket.SocketListener
import com.artem.tusaandroid.theme.TusaAndroidTheme
import com.google.accompanist.systemuicontroller.rememberSystemUiController
import com.google.firebase.messaging.FirebaseMessaging
import com.squareup.moshi.Moshi
import dagger.hilt.android.AndroidEntryPoint
import javax.inject.Inject

@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    @Inject
    lateinit var profileState: ProfileState
    @Inject
    lateinit var lastLocationState: LastLocationState
    @Inject
    lateinit var socketConnect: SocketConnect
    @Inject
    lateinit var moshi: Moshi
    @Inject
    lateinit var customTucikEndpoints: CustomTucikEndpoints
    @Inject
    lateinit var firebaseState: FirebaseState
    @Inject
    lateinit var socketListener: SocketListener


    @OptIn(ExperimentalStdlibApi::class)
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val sharedPreferences: SharedPreferences = getSharedPreferences("basic", Context.MODE_PRIVATE)
        profileState.load(sharedPreferences, -1)
        lastLocationState.load(sharedPreferences, profileState.getUserId())

        enableEdgeToEdge(
            statusBarStyle = SystemBarStyle.dark(Color.Transparent.toArgb()),
            navigationBarStyle = SystemBarStyle.dark(Color.Transparent.toArgb())
        )

        FirebaseMessaging.getInstance().token.addOnCompleteListener { task ->
            if (task.isSuccessful) {
                val token = task.result
                Log.d("FCM", "FCM Token: $token")
                // Отправь токен на сервер
                firebaseState.saveToken(token)
            }
        }

        setContent {
            TusaAndroidTheme() {
                NotificationsEnabledCheck(
                    model = TucikViewModel(preview = false,
                        previewModel = NotificationsEnabledCheckViewModelPreview())
                )
                // когда мы получаем от сервера историю действий над сущностями то обновляем локальное состояние базы данных
                // синхронизация локальной базы данных с сервером
                // он это делает только если состояние уже есть в локальной базе данных
                RefreshStateListeners(model = hiltViewModel<RefreshStateListenersViewModel>())

                // загружаем все локации друзей каждый раз когда соединение открывается
                LoadAllFriendsLocations(model = hiltViewModel())

                // слушаем обновления локации друзей
                // загружаем аватрки этих друзей чтобы на карте их показать
                ListenLocationsUpdates(model = hiltViewModel())

                Tucik()

                // события жизненного цикла кроме OnRestart
                TucikLifecycleEvents(model = hiltViewModel())

            }
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
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
    val systemUiController = rememberSystemUiController()

    val uiShouldBeLight = model.systemUIState.getIsLight()
    LaunchedEffect(uiShouldBeLight.value) {
        if (uiShouldBeLight.value) {
            systemUiController.setStatusBarColor(
                color = Color.Transparent,
                darkIcons = false
            )
            systemUiController.setNavigationBarColor(
                color = Color.Transparent,
                darkIcons = false,
            )
            return@LaunchedEffect
        }

        systemUiController.setStatusBarColor(
            color = Color.Transparent,
            darkIcons = true
        )
        systemUiController.setNavigationBarColor(
            color = Color.Transparent,
            darkIcons = true
        )
    }


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

            MapTitle(
                modifier = Modifier
                    .align(Alignment.TopStart)
                    .padding(13.dp)
                    .padding(top = 21.dp),
                model = hiltViewModel()
            )

            if (model.authenticationState.authenticated) {
                // может достать аватар из room
                // если нету то будет пробовать его загрузить
                // после поставит флаг на обновление аватара в рендере
                RetrieveMyAvatar(model = hiltViewModel())

                // если у пользователя не введено уникальное имя
                // то предлогаем ему его ввести
                InputUniqueName(hiltViewModel())

                Row(
                    modifier = Modifier
                        .align(Alignment.BottomCenter)
                        .padding(bottom = 10.dp),
                    verticalAlignment = Alignment.Bottom
                ) {
                    FriendsActionFab(
                        modifier = Modifier.padding(4.dp),
                        model = hiltViewModel()
                    )


                    MainActionFab(modifier = Modifier
                        .width(90.dp)
                        .height(90.dp)
                        .padding(4.dp),
                        model = hiltViewModel()
                    )

                    ChatsActionFab(
                        modifier = Modifier.padding(4.dp),
                        model = hiltViewModel()
                    )
                }

                Column(
                    modifier = Modifier
                        .align(Alignment.BottomEnd)
                        .padding(bottom = 10.dp).padding(end = 21.dp),
                ) {
                    val lastLocationExistsState = model.lastLocationState.getLastLocationExists()
                    if (lastLocationExistsState.value) {
                        MoveToMyLocationFab(
                            modifier = Modifier,
                            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMoveToMyLocationViewModel())
                        )
                        Spacer(modifier = Modifier.height(10.dp))
                    }
                }


                if (model.profileState.getUserId() == 1L) {
                    Column {
                        LogsFab(
                            modifier = Modifier
                                .padding(8.dp),
                            model = TucikViewModel(preview = model.isPreview(), previewModel = LogsFabViewModelPreview())
                        )
                        AdminFab(modifier = Modifier
                            .padding(8.dp),
                            hiltViewModel()
                        )
                    }
                }

                SelectedMarkerModal(model = hiltViewModel())

                ChatModal(chatViewModel = hiltViewModel())

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