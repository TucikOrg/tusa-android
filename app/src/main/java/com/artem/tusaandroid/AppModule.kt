package com.artem.tusaandroid

import android.content.Context
import com.artem.tusaandroid.api.AuthenticationControllerApi
import com.artem.tusaandroid.api.AvatarControllerApi
import com.artem.tusaandroid.api.LegalControllerApi
import com.artem.tusaandroid.api.LocationControllerApi
import com.artem.tusaandroid.api.ProfileControllerApi
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.app.profile.ProfileState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.requests.auth.AuthorizationInterceptor
import dagger.Module
import dagger.Provides
import dagger.hilt.InstallIn
import dagger.hilt.android.qualifiers.ActivityContext
import dagger.hilt.android.scopes.ActivityScoped
import dagger.hilt.components.SingletonComponent
import okhttp3.OkHttpClient
import javax.inject.Singleton

@Module
@InstallIn(SingletonComponent::class)
class AppModule {
    private val basePath = "http://192.168.0.103:8080"

    @Provides
    @Singleton
    fun provideCustomTucikEndpoints(): CustomTucikEndpoints {
        return CustomTucikEndpoints(basePath)
    }

    @Provides
    @Singleton
    fun provideMeAvatarState(): MeAvatarState {
        return MeAvatarState()
    }

    @Provides
    @Singleton
    fun provideProfileState(): ProfileState {
        return ProfileState()
    }

    @Provides
    @Singleton
    fun provideLastLocationState(): LastLocationState {
        return LastLocationState()
    }

    @Provides
    @Singleton
    fun provideAuthenticationState(
        profileState: ProfileState,
        client: OkHttpClient,
        customTucikEndpoints: CustomTucikEndpoints
    ): AuthenticationState {
        return AuthenticationState(profileState, client, customTucikEndpoints)
    }

    @Provides
    @Singleton
    fun provideClient(profileState: ProfileState): OkHttpClient {
        return OkHttpClient.Builder()
            .addInterceptor(AuthorizationInterceptor(profileState))
            .build()
    }

    @Provides
    @Singleton
    fun provideAuthenticationController(): AuthenticationControllerApi {
        return AuthenticationControllerApi(basePath)
    }

    @Provides
    @Singleton
    fun provideAvatarControllerApi(client: OkHttpClient): AvatarControllerApi {
        return AvatarControllerApi(basePath, client)
    }

    @Provides
    @Singleton
    fun provideLegalControllerApi(client: OkHttpClient): LegalControllerApi {
        return LegalControllerApi(basePath, client)
    }

    @Provides
    @Singleton
    fun provideLocationControllerApi(client: OkHttpClient): LocationControllerApi {
        return LocationControllerApi(basePath, client)
    }

    @Provides
    @Singleton
    fun provideProfileControllerApi(client: OkHttpClient): ProfileControllerApi {
        return ProfileControllerApi(basePath, client)
    }
}