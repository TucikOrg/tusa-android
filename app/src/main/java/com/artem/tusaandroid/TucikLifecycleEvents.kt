package com.artem.tusaandroid

import androidx.compose.runtime.Composable
import androidx.compose.runtime.DisposableEffect
import androidx.compose.ui.platform.LocalContext
import androidx.lifecycle.Lifecycle
import androidx.lifecycle.LifecycleEventObserver
import androidx.lifecycle.compose.LocalLifecycleOwner

@Composable
fun TucikLifecycleEvents(model: TucikLifecycleEventsViewModel) {
    val lifecycleOwner = LocalLifecycleOwner.current
    val context = LocalContext.current
    DisposableEffect(lifecycleOwner) {
        val observer = LifecycleEventObserver { _, event ->
            when(event) {
                Lifecycle.Event.ON_STOP -> {
                    // чтобы интернет трафик поберечь отключаем сокет в ситуации сворачивания приложения
                    model.socketListener.disconnect()
                }
                Lifecycle.Event.ON_START -> {
                    model.makeOnStart(context)
                }
                Lifecycle.Event.ON_CREATE -> {

                }
                Lifecycle.Event.ON_RESUME -> {

                }
                Lifecycle.Event.ON_PAUSE -> {

                }
                Lifecycle.Event.ON_DESTROY -> {
                    // вызывается при полном зактытии приложения
                    NativeLibrary.cleanup()
                }
                Lifecycle.Event.ON_ANY -> {

                }
            }
        }
        val lifecycle = lifecycleOwner.lifecycle
        lifecycle.addObserver(observer)
        onDispose {
            lifecycle.removeObserver(observer)
        }
    }
}