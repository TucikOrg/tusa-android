package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class IsMyLocationVisibleStateDto(
    val isMyLocationVisible: Boolean,
    val ownerId: Long
)