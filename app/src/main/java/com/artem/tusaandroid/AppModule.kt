package com.artem.tusaandroid

import android.content.Context
import android.content.res.AssetManager
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.IsOnlineState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.app.chat.ChatsState
import com.artem.tusaandroid.app.dialog.AppDialogState
import com.artem.tusaandroid.app.image.ImageDao
import com.artem.tusaandroid.app.image.ImageState
import com.artem.tusaandroid.app.image.TempIdToUriDao
import com.artem.tusaandroid.app.image.preview.ImagesPreviewDialogState
import com.artem.tusaandroid.app.logs.AppLogsState
import com.artem.tusaandroid.app.map.UpdateMapTitleState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.selected.SelectedState
import com.artem.tusaandroid.app.systemui.SystemUIState
import com.artem.tusaandroid.cropper.CropperState
import com.artem.tusaandroid.firebase.FirebaseState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationUpdatingState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.notification.NotificationsService
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.requests.auth.AuthorizationInterceptor
import com.artem.tusaandroid.room.AvatarDao
import com.artem.tusaandroid.room.FriendDao
import com.artem.tusaandroid.room.messenger.ChatDao
import com.artem.tusaandroid.room.messenger.MessageDao
import com.artem.tusaandroid.socket.SocketConnect
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketListener
import com.google.android.gms.location.LocationServices
import com.squareup.moshi.Moshi
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ApplicationContext
import dagger.hilt.components.SingletonComponent
import okhttp3.OkHttpClient
import javax.inject.Singleton

@Module
@InstallIn(SingletonComponent::class)
class AppModule {
    @Provides
    @Singleton
    fun provideImagesPreviewDialogState(): ImagesPreviewDialogState {
        return ImagesPreviewDialogState()
    }

    @Provides
    @Singleton
    fun provideImageState(
        socketListener: SocketListener,
        imageDao: ImageDao,
        tempIdToUriDao: TempIdToUriDao
    ): ImageState {
        return ImageState(socketListener, imageDao, tempIdToUriDao)
    }

    @Provides
    @Singleton
    fun provideIsOnlineState(socketListener: SocketListener): IsOnlineState {
        return IsOnlineState(socketListener)
    }

    @Provides
    @Singleton
    fun provideFirebaseState(socketListener: SocketListener): FirebaseState {
        return FirebaseState(socketListener)
    }

    @Provides
    @Singleton
    fun provideUpdateMapTitleState(): UpdateMapTitleState {
        return UpdateMapTitleState()
    }

    @Provides
    @Singleton
    fun provideSocketConnect(
        profileState: ProfileState,
        socketListener: SocketListener
    ): SocketConnect {
        return SocketConnect(profileState, socketListener)
    }

    @Provides
    @Singleton
    fun provideCameraState(
        systemUIState: SystemUIState
    ): CameraState {
        return CameraState(systemUIState)
    }

    @Provides
    @Singleton
    fun provideAppLogsState(): AppLogsState {
        return AppLogsState()
    }

    @Provides
    @Singleton
    fun provideLocationUpdatingState(
        @ApplicationContext appContext: Context,
        lastLocationState: LastLocationState,
        customTucikEndpoints: CustomTucikEndpoints,
        moshi: Moshi,
        okHttpClient: OkHttpClient,
        appLogsState: AppLogsState
    ): LocationUpdatingState {
        val fusedLocationClient = LocationServices.getFusedLocationProviderClient(appContext)
        return LocationUpdatingState(
            fusedLocationClient = fusedLocationClient,
            lastLocationState = lastLocationState,
            customTucikEndpoints = customTucikEndpoints,
            moshi = moshi,
            okHttpClient = okHttpClient,
            appLogsState = appLogsState
        )
    }

    @Provides
    @Singleton
    fun provideSystemUIState(): SystemUIState {
        return SystemUIState()
    }

    @Provides
    @Singleton
    fun providerAppDialogState(): AppDialogState {
        return AppDialogState()
    }

    @Provides
    @Singleton
    fun provideChatsState(
        socketListener: SocketListener
    ): ChatsState {
        return ChatsState(socketListener)
    }

    @Provides
    @Singleton
    fun provideChatState(
        profileState: ProfileState,
        chatDao: ChatDao,
        friendDao: FriendDao,
        socketListener: SocketListener,
        messagesDao: MessageDao
    ): ChatState {
        return ChatState(
            profileState,
            chatDao,
            friendDao,
            socketListener,
            messagesDao
        )
    }

    @Provides
    @Singleton
    fun providerLocationState(): LocationsState {
        return LocationsState()
    }

    @Provides
    @Singleton
    fun provideSelectedState(): SelectedState {
        return SelectedState()
    }

    @Provides
    @Singleton
    fun provideNotificationsService(): NotificationsService {
        return NotificationsService()
    }

    @Provides
    @Singleton
    fun provideCustomTucikEndpoints(): CustomTucikEndpoints {
        return CustomTucikEndpoints(BuildConfig.SERVICE_URL)
    }

    @Provides
    @Singleton
    fun provideMeAvatarState(
        profileState: ProfileState,
        avatarState: AvatarState,
        avatarDao: AvatarDao
    ): MeAvatarState {
        return MeAvatarState(
            profileState,
            avatarState,
            avatarDao
        )
    }

    @Provides
    @Singleton
    fun provideCropperState(
        dialogState: AppDialogState
    ): CropperState {
        return CropperState(dialogState)
    }

    @Provides
    @Singleton
    fun provideSocketListener(
        okHttpClient: OkHttpClient,
        socketConnectionState: SocketConnectionState,
    ): SocketListener {
        return SocketListener(
            BuildConfig.SOCKET_URL,
            okHttpClient,
            socketConnectionState
        )
    }

    @Provides
    @Singleton
    fun provideMoshi(): Moshi {
        return Moshi.Builder().add(KotlinJsonAdapterFactory()).build()
    }

    @Provides
    @Singleton
    fun provideProfileState(socketListener: SocketListener): ProfileState {
        return ProfileState(socketListener)
    }

    @Provides
    @Singleton
    fun provideLastLocationState(): LastLocationState {
        return LastLocationState()
    }

    @Provides
    @Singleton
    fun provideFriendsState(
        socketListener: SocketListener,
        locationsState: LocationsState
    ): FriendsState {
        return FriendsState(socketListener, locationsState)
    }

    @Provides
    @Singleton
    fun provideAuthenticationState(
        client: OkHttpClient,
        customTucikEndpoints: CustomTucikEndpoints,
        profileState: ProfileState,
        meAvatarState: MeAvatarState,
        moshi: Moshi,
        socketConnect: SocketConnect,
        appDialogState: AppDialogState
    ): AuthenticationState {
        return AuthenticationState(
            client, customTucikEndpoints, profileState,
            meAvatarState, moshi, socketConnect, appDialogState
        )
    }

    @Provides
    @Singleton
    fun provideSocketConnectionState(): SocketConnectionState {
        return SocketConnectionState()
    }

    @Provides
    @Singleton
    fun provideClient(): OkHttpClient {
        return OkHttpClient.Builder()
            .addInterceptor(AuthorizationInterceptor())
            .build()
    }

    @Provides
    @Singleton
    fun provideAvatarState(
        socketListener: SocketListener,
        avatarDao: AvatarDao,
        socketConnectionState: SocketConnectionState,
        locationsState: LocationsState,
        assetManager: AssetManager
    ): AvatarState {
        return AvatarState(socketListener, avatarDao, socketConnectionState, locationsState, assetManager)
    }

    @Provides
    @Singleton
    fun provideAssetManager(@ApplicationContext context: Context): AssetManager {
        return context.assets
    }
}