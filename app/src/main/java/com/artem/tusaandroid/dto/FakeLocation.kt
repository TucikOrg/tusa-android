package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class FakeLocation(
    val latitude: Float,
    val longitude: Float,
    val userId: Long
)