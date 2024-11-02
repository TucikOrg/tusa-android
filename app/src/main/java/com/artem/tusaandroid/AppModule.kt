package com.artem.tusaandroid

import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.location.LastLocationState
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
//    private val basePath = "http://192.168.0.103:8080"
//    private val socketUrl = "ws://192.168.0.103:8080/stream"

    private val basePath = "https://tucik.fun"
    private val socketUrl = "wss://tucik.fun/stream"

    @Provides
    @Singleton
    fun provideCustomTucikEndpoints(): CustomTucikEndpoints {
        return CustomTucikEndpoints(basePath)
    }

    @Provides
    @Singleton
    fun provideMeAvatarState(
        profileState: ProfileState,
        socketListener: SocketListener,
        avatarState: AvatarState
    ): MeAvatarState {
        return MeAvatarState(
            profileState,
            socketListener,
            avatarState
        )
    }

    @Provides
    @Singleton
    fun provideSocketListener(
        okHttpClient: OkHttpClient,
        socketConnectionState: SocketConnectionState,
    ): SocketListener {
        return SocketListener(
            socketUrl,
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
        friendsState: FriendsState,
        socketListener: SocketListener
    ): AuthenticationState {
        return AuthenticationState(client, customTucikEndpoints, profileState,
            meAvatarState, friendsState, moshi, socketListener
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