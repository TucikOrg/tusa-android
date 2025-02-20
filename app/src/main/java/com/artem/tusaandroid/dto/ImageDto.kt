package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class ImageDto(
    val ownerId: Long,
    val localFilePathId: String,
    val image: ByteArray?
)