package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class LocationDto(
    val ownerId: Long,
    var latitude: Float,
    var longitude: Float
)