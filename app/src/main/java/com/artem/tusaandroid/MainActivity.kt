package com.artem.tusaandroid

import android.Manifest
import android.content.Context
import android.content.Intent
import android.content.SharedPreferences
import android.content.pm.PackageManager
import android.location.LocationManager
import android.os.Bundle
import androidx.activity.ComponentActivity
import androidx.activity.SystemBarStyle
import androidx.activity.compose.setContent
import androidx.activity.enableEdgeToEdge
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.width
import androidx.compose.material3.Scaffold
import androidx.compose.material3.SnackbarHost
import androidx.compose.material3.SnackbarHostState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.LaunchedEffect
import androidx.compose.runtime.remember
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.toArgb
import androidx.compose.ui.tooling.preview.Preview
import androidx.compose.ui.unit.dp
import androidx.core.app.ActivityCompat
import androidx.hilt.navigation.compose.hiltViewModel
import com.artem.tusaandroid.app.action.MainActionFab
import com.artem.tusaandroid.app.MainActivityViewModel
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.TestInfoLine
import com.artem.tusaandroid.app.action.AdminFab
import com.artem.tusaandroid.app.action.auth.CredentialsManagerAuth
import com.artem.tusaandroid.app.action.chats.ChatsActionFab
import com.artem.tusaandroid.app.action.chats.ChatsViewModelPreview
import com.artem.tusaandroid.app.action.friends.FriendsActionFab
import com.artem.tusaandroid.app.action.friends.PreviewFriendViewModel
import com.artem.tusaandroid.app.action.logs.LogsFab
import com.artem.tusaandroid.app.action.logs.LogsFabViewModelPreview
import com.artem.tusaandroid.app.avatar.AvatarDTO
import com.artem.tusaandroid.app.chat.ChatModal
import com.artem.tusaandroid.app.dialog.AppDialog
import com.artem.tusaandroid.app.dialog.AppDialogState
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
import com.artem.tusaandroid.location.LocationForegroundService
import com.artem.tusaandroid.location.MoveToMyLocationFab
import com.artem.tusaandroid.location.PreviewMoveToMyLocationViewModel
import com.artem.tusaandroid.notification.NotificationsEnabledCheck
import com.artem.tusaandroid.notification.NotificationsEnabledCheckViewModelPreview
import com.artem.tusaandroid.socket.ConnectionStatus
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.PreviewConnectionStatusViewModel
import com.artem.tusaandroid.socket.SocketListener
import com.artem.tusaandroid.theme.TusaAndroidTheme
import com.google.accompanist.systemuicontroller.rememberSystemUiController
import com.google.android.gms.common.ConnectionResult
import com.google.android.gms.common.GoogleApiAvailability
import com.google.android.gms.fitness.LocalRecordingClient
import dagger.hilt.android.AndroidEntryPoint
import javax.inject.Inject

@AndroidEntryPoint
class MainActivity : ComponentActivity() {
    @Inject
    lateinit var profileState: ProfileState
    @Inject
    lateinit var lastLocationState: LastLocationState
    @Inject
    lateinit var socketListener: SocketListener
    @Inject
    lateinit var meAvatarState: MeAvatarState
    @Inject
    lateinit var appDialogState: AppDialogState

    private val listener = object: EventListener<AvatarDTO> {
        override fun onEvent(event: AvatarDTO) {
            val myAvatar = profileState.getUserId() == event.ownerId
            if (myAvatar) {
                meAvatarState.updateMeMarkerInRender()
                socketListener.getReceiveMessage().avatarBus.removeListener(this)
            }
        }
    }


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
                NotificationsEnabledCheck(
                    model = TucikViewModel(preview = false,
                        previewModel = NotificationsEnabledCheckViewModelPreview())
                )
                Tucik()
            }
        }
    }

    override fun onNewIntent(intent: Intent) {
        super.onNewIntent(intent)
    }

    // вызывается при полном зактытии приложения
    override fun onDestroy() {
        NativeLibrary.cleanup()
        super.onDestroy()
    }

    override fun onStart() {
        super.onStart()
        makeOnStart()
    }

    override fun onResume() {
        super.onResume()
    }

    override fun onRestart() {
        // вызывается только когда возвращается свернутое приложение
        // при возвращении в приложение включаем сокет Он снова нужен для общения
        socketListener.connect(null)
        super.onRestart()
    }

    override fun onStop() {
        // чтобы интернет трафик поберечь отключаем сокет в ситуации сворачивания приложения
        socketListener.disconnect()
        super.onStop()
    }


    fun makeOnStart() {
        // нету нужного гугл сервиса
        val hasMinPlayServices = GoogleApiAvailability.getInstance()
            .isGooglePlayServicesAvailable(this, LocalRecordingClient.LOCAL_RECORDING_CLIENT_MIN_VERSION_CODE)
        if(hasMinPlayServices != ConnectionResult.SUCCESS) {
            // Prompt user to update their device's Google Play services app and return
            appDialogState.open("Google Play Servcies", "Для коректной работы приложения необходимо обновить Google Play Services")
        }

        val locationPermissionGranted = ActivityCompat.checkSelfPermission(
            this,
            android.Manifest.permission.ACCESS_FINE_LOCATION
        ) == PackageManager.PERMISSION_GRANTED

        val activityRecognitionPermissionGranted = ActivityCompat.checkSelfPermission(
            this,
            Manifest.permission.ACTIVITY_RECOGNITION
        ) == PackageManager.PERMISSION_GRANTED

        if ( locationPermissionGranted && activityRecognitionPermissionGranted ) {
            // Если сервис был запущен и разрешение на геолокацию есть и он был запущен, то запускаем сервис
            // это еще так же значит что пользователь желает отображать себя на карте
            // + gps включен
            val locationManager = getSystemService(Context.LOCATION_SERVICE) as LocationManager
            val gpsEnabled = locationManager.isProviderEnabled(LocationManager.GPS_PROVIDER)
            if (lastLocationState.getLocationForegroundServiceStarted() == true && gpsEnabled) {
                val startIntent = Intent(this, LocationForegroundService::class.java).apply {
                    action = LocationForegroundService.ACTION_START
                    flags = Intent.FLAG_ACTIVITY_SINGLE_TOP or Intent.FLAG_ACTIVITY_CLEAR_TOP
                }
                startForegroundService(startIntent)


                meAvatarState.updateMeMarkerInRender()
                if (meAvatarState.hasAvatar() == false) {
                    // грузим автар пользователя и потом добавляем маркер
                    socketListener.getReceiveMessage().avatarBus.addListener(listener)
                }
            }
        } else {
            // Если сервис был запущен, но разрешение на геолокацию или фитнесс отозвали, то останавливаем сервис
            lastLocationState.saveLocationForegroundServiceStarted(false)
        }

        if (profileState.getIsAuthenticated() == true) {
            profileState.getUserId().let {
                socketListener.connect(it)
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

            if (model.authenticationState.authenticated) {
                InputUniqueName(hiltViewModel())

                MainActionFab(modifier = Modifier
                    .align(Alignment.BottomCenter)
                    .width(120.dp)
                    .height(120.dp)
                    .padding(16.dp),
                    hiltViewModel()
                )

                if (model.profileState.getUserId() == 1L || true) {
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

                Column(
                    modifier = Modifier
                        .align(Alignment.BottomEnd)
                        .padding(16.dp)
                ) {
                    val lastLocationExistsState = model.lastLocationState.getLastLocationExists()
                    if (lastLocationExistsState.value) {
                        MoveToMyLocationFab(
                            modifier = Modifier,
                            model = TucikViewModel(preview = model.isPreview(), previewModel = PreviewMoveToMyLocationViewModel())
                        )
                        Spacer(modifier = Modifier.height(10.dp))
                    }
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