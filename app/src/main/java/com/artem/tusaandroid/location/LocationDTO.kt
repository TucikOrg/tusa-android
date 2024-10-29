package com.artem.tusaandroid.location

import kotlinx.serialization.Serializable

@Serializable
data class LocationDto(
    val ownerId: Long,
    val latitude: Float,
    val longitude: Float
)