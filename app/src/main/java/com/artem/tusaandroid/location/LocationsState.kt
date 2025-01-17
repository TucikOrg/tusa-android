package com.artem.tusaandroid.location

import com.artem.tusaandroid.NativeLibrary

class LocationsState() {
    var friendLocations: MutableList<FriendLocation> = mutableListOf()

    fun getLocation(markerId: Long): FriendLocation? {
        return friendLocations.find { it.ownerId == markerId }
    }

    fun removeLocation(markerId: kotlin.Long) {
        friendLocations.removeIf { it.ownerId == markerId }
        NativeLibrary.removeMarker(markerId)
    }
}