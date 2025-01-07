package com.artem.tusaandroid.cropper

import android.graphics.Bitmap
import androidx.compose.runtime.mutableStateOf
import androidx.compose.ui.geometry.Offset
import com.artem.tusaandroid.app.dialog.AppDialogState

class CropperState(
    private val dialog: AppDialogState
) {
    val cropperOpened = mutableStateOf(false)
    private var bitmap: Bitmap? = null
    private var croppedBitmap: Bitmap? = null
    private var onCropped: ((Bitmap?) -> Unit)? = null

    fun getBitmap() = bitmap

    fun openCropper(bitmap: Bitmap, onCropped: (Bitmap?) -> Unit) {
        if (bitmap.width < 512 || bitmap.height < 512) {
            dialog.open(title = "Картинка не подходит", message = "Должна быть не менее 512x512")
            return
        }

        this.bitmap = bitmap
        cropperOpened.value = true
        this.onCropped = onCropped
    }

    fun crop(scale: Float, transform: Offset, cropSize: Float) {
        val scaleInv = (1 / scale)
        val size = (cropSize * scaleInv).toInt()
        croppedBitmap = Bitmap.createScaledBitmap(Bitmap.createBitmap(
            bitmap!!,
            -(transform.x * scaleInv).toInt(),
            -(transform.y * scaleInv).toInt(),
            size,
            size
        ), 256, 256, true)

        onCropped?.invoke(croppedBitmap)
        cropperOpened.value = false
    }

    companion object {
        var previewState: CropperState? = null

        fun preview(): CropperState {
            if (previewState == null) {
                previewState = CropperState(AppDialogState())
            }
            return previewState!!
        }
    }
}