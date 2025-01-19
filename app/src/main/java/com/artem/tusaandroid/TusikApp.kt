package com.artem.tusaandroid

import android.app.Application
import android.os.StrictMode
import android.util.Log
import com.artem.tusaandroid.dto.AddLocationDto
import com.artem.tusaandroid.dto.CrashData
import com.artem.tusaandroid.requests.CustomTucikEndpoints
import com.squareup.moshi.Moshi
import com.squareup.moshi.adapter
import dagger.hilt.android.HiltAndroidApp
import kotlinx.coroutines.CoroutineExceptionHandler
import kotlinx.coroutines.DelicateCoroutinesApi
import kotlinx.coroutines.Dispatchers
import kotlinx.coroutines.GlobalScope
import kotlinx.coroutines.launch
import okhttp3.MediaType.Companion.toMediaTypeOrNull
import okhttp3.OkHttpClient
import okhttp3.Request
import okhttp3.RequestBody.Companion.toRequestBody
import javax.inject.Inject
import kotlin.system.exitProcess

@HiltAndroidApp
class TusikApp: Application() {
    @Inject
    lateinit var okHttpClient: OkHttpClient
    @Inject
    lateinit var customTucikEndpoints: CustomTucikEndpoints
    @Inject
    lateinit var moshi: Moshi


    override fun onCreate() {
        super.onCreate()

        // Устанавливаем глобальный обработчик исключений
        Thread.setDefaultUncaughtExceptionHandler { thread, throwable ->
            val policy = StrictMode.ThreadPolicy.Builder().permitNetwork().build()
            StrictMode.setThreadPolicy(policy)

            // Логируем ошибку
            Log.e("CrashHandler", "Uncaught exception in thread ${thread.name}", throwable)

            // Отправляем данные об ошибке на сервер
            sendCrashToServer(throwable)

            // Завершаем приложение
            exitProcess(1)
        }
    }

    @OptIn(ExperimentalStdlibApi::class, DelicateCoroutinesApi::class)
    private fun sendCrashToServer(throwable: Throwable) {
        val request = Request.Builder()
            .url(customTucikEndpoints.makeSendLogs())
            .post(
                moshi.adapter<CrashData>().toJson(CrashData(
                    message = throwable.message ?: "",
                    stackTrace = throwable.stackTraceToString(),
                    thread = Thread.currentThread().name
                )).toRequestBody("application/json".toMediaTypeOrNull())
            )
            .build()
        try {
            okHttpClient.newCall(request).execute()
        } catch (exception: Exception) {
            exception.printStackTrace()
        }
    }
}
