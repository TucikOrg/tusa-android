package com.artem.tusaandroid.dto

import kotlinx.serialization.Serializable

@Serializable
class AvatarDTO(
    val ownerId: Long,
    val avatar: ByteArray?
)