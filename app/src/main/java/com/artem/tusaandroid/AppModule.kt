package com.artem.tusaandroid

import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.StartStopApp
import com.artem.tusaandroid.app.action.friends.FriendsState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.requests.auth.AuthorizationInterceptor
import com.artem.tusaandroid.socket.ReceiveMessage
import com.artem.tusaandroid.socket.SendMessage
import com.artem.tusaandroid.socket.WebSocketClient
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
    private val basePath = "http://192.168.0.103:8080"
    private val socketUrl = "ws://192.168.0.103:8080/stream"

    @Provides
    @Singleton
    fun provideCustomTucikEndpoints(): CustomTucikEndpoints {
        return CustomTucikEndpoints(basePath)
    }

    @Provides
    @Singleton
    fun provideMeAvatarState(
        profileState: ProfileState,
        sendMessage: SendMessage,
        receiveMessage: ReceiveMessage
    ): MeAvatarState {
        return MeAvatarState(
            profileState,
            sendMessage,
            receiveMessage
        )
    }

    @Provides
    @Singleton
    fun provideChatWebSocketClient(
        okHttpClient: OkHttpClient,
        receiveMessage: ReceiveMessage
    ): WebSocketClient {
        return WebSocketClient(okHttpClient, socketUrl, receiveMessage)
    }

    @Provides
    @Singleton
    fun provideReceiveMessage(): ReceiveMessage {
        return ReceiveMessage()
    }

    @Provides
    @Singleton
    fun provideSendMessage(socketClient: WebSocketClient): SendMessage {
        return SendMessage(socketClient, )
    }

    @Provides
    @Singleton
    fun provideMoshi(): Moshi {
        return Moshi.Builder().add(KotlinJsonAdapterFactory()).build()
    }

    @Provides
    @Singleton
    fun provideProfileState(sendMessage: SendMessage): ProfileState {
        return ProfileState(sendMessage)
    }

    @Provides
    @Singleton
    fun provideLastLocationState(): LastLocationState {
        return LastLocationState()
    }

    @Provides
    @Singleton
    fun provideFriendsState(
        sendMessage: SendMessage,
        receiveMessage: ReceiveMessage
    ): FriendsState {
        return FriendsState(sendMessage, receiveMessage)
    }

    @Provides
    @Singleton
    fun provideAuthenticationState(
        client: OkHttpClient,
        customTucikEndpoints: CustomTucikEndpoints,
        profileState: ProfileState,
        meAvatarState: MeAvatarState,
        moshi: Moshi,
        startStopApp: StartStopApp
    ): AuthenticationState {
        return AuthenticationState(client, customTucikEndpoints, profileState, meAvatarState, moshi, startStopApp)
    }

    @Provides
    @Singleton
    fun provideStartStopApp(
        meAvatarState: MeAvatarState,
        friendsState: FriendsState,
        webSocketClient: WebSocketClient
    ): StartStopApp {
        return StartStopApp(meAvatarState, friendsState, webSocketClient)
    }

    @Provides
    @Singleton
    fun provideClient(): OkHttpClient {
        return OkHttpClient.Builder()
            .addInterceptor(AuthorizationInterceptor())
            .build()
    }
}