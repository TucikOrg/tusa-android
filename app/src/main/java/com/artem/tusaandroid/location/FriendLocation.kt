package com.artem.tusaandroid.location

data class FriendLocation(
    var longitude: Float,
    var latitude: Float,
    val ownerId: Long,
    var updateAvatar: Boolean,
    var updateMarkerFlag: Boolean
)