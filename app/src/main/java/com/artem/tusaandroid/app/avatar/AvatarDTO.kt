package com.artem.tusaandroid.app.avatar

import kotlinx.serialization.Serializable

@Serializable
class AvatarDTO(
    val ownerId: Long,
    val avatar: ByteArray
)