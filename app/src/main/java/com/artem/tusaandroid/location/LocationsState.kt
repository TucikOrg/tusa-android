package com.artem.tusaandroid.location

import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener

class LocationsState(
    private val socketListener: SocketListener
) {
    init {
        socketListener.getReceiveMessage().locationsBus.addListener(object: EventListener<List<LocationDto>> {
            override fun onEvent(event: List<LocationDto>) {
                currentLocations = event
            }
        })
    }

    var currentLocations: List<LocationDto> = listOf()

    fun getLocation(markerId: Long): LocationDto? {
        return currentLocations.find { it.ownerId == markerId }
    }
}