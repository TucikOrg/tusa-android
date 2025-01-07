package com.artem.tusaandroid.cropper

import androidx.compose.foundation.Canvas
import androidx.compose.foundation.gestures.detectTransformGestures
import androidx.compose.foundation.layout.Arrangement
import androidx.compose.foundation.layout.Box
import androidx.compose.foundation.layout.Column
import androidx.compose.foundation.layout.Row
import androidx.compose.foundation.layout.Spacer
import androidx.compose.foundation.layout.WindowInsets
import androidx.compose.foundation.layout.fillMaxSize
import androidx.compose.foundation.layout.fillMaxWidth
import androidx.compose.foundation.layout.height
import androidx.compose.foundation.layout.navigationBarsPadding
import androidx.compose.foundation.layout.padding
import androidx.compose.foundation.layout.safeDrawing
import androidx.compose.foundation.layout.width
import androidx.compose.foundation.layout.windowInsetsPadding
import androidx.compose.material3.Button
import androidx.compose.material3.ExperimentalMaterial3Api
import androidx.compose.material3.Icon
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.ModalBottomSheet
import androidx.compose.material3.ModalBottomSheetProperties
import androidx.compose.material3.Text
import androidx.compose.material3.rememberModalBottomSheetState
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.runtime.mutableFloatStateOf
import androidx.compose.runtime.mutableStateOf
import androidx.compose.runtime.remember
import androidx.compose.runtime.setValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.draw.clipToBounds
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.asImageBitmap
import androidx.compose.ui.graphics.drawscope.withTransform
import androidx.compose.ui.input.pointer.pointerInput
import androidx.compose.ui.platform.LocalConfiguration
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.res.painterResource
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextAlign
import androidx.compose.ui.unit.dp
import com.artem.tusaandroid.R

@OptIn(ExperimentalMaterial3Api::class)
@Composable
fun CropperModal(model: CropperModalViewModel) {
    if (model.getShowState().value) {
        val density = LocalDensity.current

        val configuration = LocalConfiguration.current
        val screenWidth = configuration.screenWidthDp.dp
        val screenWidthPx = with(density) { screenWidth.toPx() }
        val image = model.getBitmap()!!.asImageBitmap()
        val imageWidth = image.width
        val imageHeight = image.height
        val screenImageWidthRatio = screenWidthPx / imageWidth

        var scale by remember {
            mutableFloatStateOf(screenImageWidthRatio)
        }
        var transform by remember {
            mutableStateOf(Offset(
                0.0f,
                0.0f
            ))
        }

        var zoomOffsetX = 0f
        var zoomOffsetY = 0f

        val sheetState = rememberModalBottomSheetState(skipPartiallyExpanded = true)
        ModalBottomSheet(
            modifier = Modifier.windowInsetsPadding(WindowInsets.safeDrawing),
            onDismissRequest = {
                model.getShowState().value = false
            },
            sheetState = sheetState,
            properties = ModalBottomSheetProperties(
                isAppearanceLightStatusBars = false
            ),
        ) {
            Column(
                modifier = Modifier.fillMaxSize(),
                verticalArrangement = Arrangement.Top,
            ) {
                Text(
                    modifier = Modifier.fillMaxWidth().padding(horizontal = 10.dp),
                    text = "Обрезать аватарку",
                    style = MaterialTheme.typography.headlineSmall,
                    fontWeight = FontWeight.Bold,
                    textAlign = TextAlign.Start
                )
                Spacer(modifier = Modifier.height(10.dp))
                Box(
                    modifier = Modifier
                        .width(screenWidth)
                        .height(screenWidth)
                        .clipToBounds()
                        .pointerInput(Unit) {
                            detectTransformGestures { centroid, pan, zoom, rotation ->
                                val newScale = scale.times(zoom)

                                val widthPass = imageWidth * newScale >= screenWidthPx
                                val heightPass = imageHeight * newScale >= screenWidthPx
                                if (widthPass && heightPass) {
                                    scale = newScale
                                }

                                val pannedX = transform.x.plus(pan.x * (1 / scale))
                                val pannedY = transform.y.plus(pan.y * (1 / scale))

                                zoomOffsetX = (scale - 1.0f) * (-pannedX + screenWidthPx / 2.0f)
                                zoomOffsetY = (scale - 1.0f) * (-pannedY + screenWidthPx / 2.0f)

                                val panXMax = imageWidth.toFloat() * scale - screenWidthPx - zoomOffsetX
                                val panYMax = imageHeight.toFloat() * scale - screenWidthPx - zoomOffsetY
                                transform = Offset(
                                    pannedX.coerceIn(-panXMax, zoomOffsetX),
                                    pannedY.coerceIn(-panYMax, zoomOffsetY),
                                )
                            }
                        }
                ) {
                    Canvas(
                        modifier = Modifier
                    ) {
                        withTransform(
                            {
                                translate(
                                    left = transform.x - zoomOffsetX,
                                    top = transform.y - zoomOffsetY,
                                )
                                scale(
                                    scaleX = scale,
                                    scaleY = scale,
                                )
                            }
                        ) {
                            drawImage(
                                image = image,
                            )
                        }
                    }
                }
                Row(
                    modifier = Modifier
                        .padding(horizontal = 10.dp, vertical = 5.dp)
                        .height(30.dp),
                    verticalAlignment = Alignment.CenterVertically
                ) {
                    Icon(
                        painter = painterResource(R.drawable.drag_pan),
                        contentDescription = "drag pan",
                    )
                    Icon(
                        painter = painterResource(R.drawable.pan_zoom),
                        contentDescription = "pan zoom"
                    )
                    Icon(
                        painter = painterResource(R.drawable.touch_app),
                        contentDescription = "touch image"
                    )
                    Spacer(modifier = Modifier.width(21.dp))
                    Text("Перемещай, зумируй картинку", fontWeight = FontWeight.Bold)
                }
                Button(
                    modifier = Modifier.fillMaxWidth().padding(horizontal = 10.dp, vertical = 5.dp),
                    onClick = {
                        model.getCropperState().crop(
                            scale = scale,
                            transform = Offset(
                                transform.x - zoomOffsetX,
                                transform.y - zoomOffsetY
                            ),
                            cropSize = screenWidthPx
                        )
                    }
                ) {
                    Text("Обрезать")
                }
            }
        }
    }
}