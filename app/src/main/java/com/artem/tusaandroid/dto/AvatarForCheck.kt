package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class AvatarForCheck(
    val ownerId: Long,
    val updatingTime: Long
)