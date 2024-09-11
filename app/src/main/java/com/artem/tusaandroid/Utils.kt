package com.artem.tusaandroid

import androidx.compose.runtime.Composable
import androidx.hilt.navigation.compose.hiltViewModel
import androidx.lifecycle.ViewModel
import com.artem.tusaandroid.app.IsPreviewViewModel
import com.google.android.gms.tasks.Task
import kotlinx.coroutines.suspendCancellableCoroutine
import kotlin.coroutines.resume
import kotlin.coroutines.resumeWithException

suspend fun <T> Task<T>.await(): T {
    return suspendCancellableCoroutine { continuation ->
        this.addOnSuccessListener { result ->
            continuation.resume(result) // Resume the coroutine with the result
        }
        this.addOnFailureListener { exception ->
            continuation.resumeWithException(exception) // Resume with exception if task fails
        }
        this.addOnCanceledListener {
            continuation.cancel() // Cancel the coroutine if the task is cancelled
        }
    }
}

fun ViewModel.isPreview(): Boolean {
    return this is IsPreviewViewModel
}

@Composable
inline fun <reified T : ViewModel> TucikViewModel(preview: Boolean, previewModel: ViewModel): T {
    return if (preview) { previewModel } else { hiltViewModel<T>() } as T
}