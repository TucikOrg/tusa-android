package com.artem.tusaandroid

import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.chat.ChatState
import com.artem.tusaandroid.app.chat.ChatsState
import com.artem.tusaandroid.app.dialog.AppDialogState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.app.selected.SelectedState
import com.artem.tusaandroid.cropper.CropperState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.location.LocationsState
import com.artem.tusaandroid.notification.NotificationsService
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.requests.auth.AuthorizationInterceptor
import com.artem.tusaandroid.socket.SocketConnectionState
import com.artem.tusaandroid.socket.SocketListener
import com.squareup.moshi.Moshi
import com.squareup.moshi.kotlin.reflect.KotlinJsonAdapterFactory
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.components.SingletonComponent
import okhttp3.OkHttpClient
import javax.inject.Singleton

@Module
@InstallIn(SingletonComponent::class)
class AppModule {

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
        socketListener: SocketListener,
        profileState: ProfileState
    ): ChatState {
        return ChatState(
            socketListener,
            profileState
        )
    }

    @Provides
    @Singleton
    fun providerLocationState(
        socketListener: SocketListener
    ): LocationsState {
        return LocationsState(socketListener)
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
        avatarState: AvatarState
    ): MeAvatarState {
        return MeAvatarState(
            profileState,
            avatarState
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
        socketListener: SocketListener
    ): FriendsState {
        return FriendsState(socketListener)
    }

    @Provides
    @Singleton
    fun provideAuthenticationState(
        client: OkHttpClient,
        customTucikEndpoints: CustomTucikEndpoints,
        profileState: ProfileState,
        meAvatarState: MeAvatarState,
        moshi: Moshi,
        socketListener: SocketListener
    ): AuthenticationState {
        return AuthenticationState(client, customTucikEndpoints, profileState,
            meAvatarState, moshi, socketListener
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
    fun provideAvatarState(socketListener: SocketListener): AvatarState {
        return AvatarState(socketListener)
    }
}