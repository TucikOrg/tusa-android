package com.artem.tusaandroid

import com.artem.tusaandroid.api.AuthenticationControllerApi
import com.artem.tusaandroid.api.AvatarControllerApi
import com.artem.tusaandroid.api.LegalControllerApi
import com.artem.tusaandroid.api.LocationControllerApi
import com.artem.tusaandroid.api.ProfileControllerApi
import com.artem.tusaandroid.app.AuthenticationState
import com.artem.tusaandroid.app.MeAvatarState
import com.artem.tusaandroid.location.LastLocationState
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.artem.tusaandroid.requests.auth.AuthorizationInterceptor
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

    @Provides
    @Singleton
    fun customTucikEndpoints(): CustomTucikEndpoints {
        return CustomTucikEndpoints(basePath)
    }

    @Provides
    @Singleton
    fun meAvatarState(): MeAvatarState {
        return MeAvatarState()
    }

    @Provides
    @Singleton
    fun provideAppVariables(): AppVariables {
        return AppVariables()
    }

    @Provides
    @Singleton
    fun provideLastLocationState(): LastLocationState {
        return LastLocationState()
    }

    @Provides
    @Singleton
    fun provideAuthenticationState(
        appVariables: AppVariables,
        client: OkHttpClient,
        customTucikEndpoints: CustomTucikEndpoints
    ): AuthenticationState {
        return AuthenticationState(appVariables, client, customTucikEndpoints)
    }

    @Provides
    @Singleton
    fun provideClient(appVariables: AppVariables): OkHttpClient {
        return OkHttpClient.Builder()
            .addInterceptor(AuthorizationInterceptor(appVariables))
            .build()
    }

    @Provides
    @Singleton
    fun authenticationController(): AuthenticationControllerApi {
        return AuthenticationControllerApi(basePath)
    }

    @Provides
    @Singleton
    fun avatarControllerApi(client: OkHttpClient): AvatarControllerApi {
        return AvatarControllerApi(basePath, client)
    }

    @Provides
    @Singleton
    fun legalControllerApi(client: OkHttpClient): LegalControllerApi {
        return LegalControllerApi(basePath, client)
    }

    @Provides
    @Singleton
    fun locationControllerApi(client: OkHttpClient): LocationControllerApi {
        return LocationControllerApi(basePath, client)
    }

    @Provides
    @Singleton
    fun profileControllerApi(client: OkHttpClient): ProfileControllerApi {
        return ProfileControllerApi(basePath, client)
    }
}