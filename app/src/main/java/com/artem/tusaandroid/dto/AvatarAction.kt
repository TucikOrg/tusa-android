package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
data class AvatarAction(
    val ownerId: Long,
    val actionType: AvatarActionType,
    val actionTime: Long
)