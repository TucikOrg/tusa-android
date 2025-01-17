package com.artem.tusaandroid.location

import androidx.lifecycle.ViewModel
import androidx.lifecycle.viewModelScope
import com.artem.tusaandroid.app.avatar.AvatarState
import com.artem.tusaandroid.dto.LocationDto
import com.artem.tusaandroid.dto.UpdateLocationDto
import com.artem.tusaandroid.socket.EventListener
import com.artem.tusaandroid.socket.SocketListener
import dagger.hilt.android.lifecycle.HiltViewModel
import javax.inject.Inject

@HiltViewModel
open class ListenLocationsUpdatesViewModel @Inject constructor(
    private val socketListener: SocketListener,
    private val locationsState: LocationsState,
    private val avatarState: AvatarState
) : ViewModel() {
    init {
        socketListener.getReceiveMessage().updateLocationBus.addListener(object: EventListener<UpdateLocationDto> {
            override fun onEvent(event: UpdateLocationDto) {
                val location = locationsState.friendLocations.find { it.ownerId == event.whoId }
                if (location != null) {
                    // если уже есть локация то обновляем ее
                    if (location.latitude == event.latitude && location.longitude == event.longitude) {
                        // если локация не изменилась то ничего не делаем
                        return
                    }

                    location.latitude = event.latitude
                    location.longitude = event.longitude
                    location.updateMarkerFlag = true
                } else {
                    // если нету то добавляем
                    locationsState.friendLocations.add(FriendLocation(
                        ownerId = event.whoId,
                        latitude = event.latitude,
                        longitude = event.longitude,
                        updateAvatar = true,
                        updateMarkerFlag = true
                    ))
                }

                // при получении обновлении локации тоже проверяем наличие аватарки
                val avatar = avatarState.retrieveAvatar(
                    userId = event.whoId,
                    scope = viewModelScope
                ) { availableAvatar ->
                    // если аватарка пришла то обновляем ее в маркере если есть
                    val location = locationsState.friendLocations.find { it.ownerId == event.whoId }
                    if (location != null) {
                        location.updateMarkerFlag = true
                        location.updateAvatar = true
                    }
                }
            }
        })

        socketListener.getReceiveMessage().locationsBus.addListener(object: EventListener<List<LocationDto>> {
            override fun onEvent(event: List<LocationDto>) {
                locationsState.friendLocations = event.map { FriendLocation(
                    ownerId = it.ownerId,
                    latitude = it.latitude,
                    longitude = it.longitude,
                    updateAvatar = true,
                    updateMarkerFlag = true
                ) }.toMutableList()

                // нужно проверить наличие аватарок для маркеров
                for (location in locationsState.friendLocations) {
                    val avatar = avatarState.retrieveAvatar(
                        userId = location.ownerId,
                        scope = viewModelScope
                    ) { available ->
                        // если аватарка пришла то обновляем ее в маркере если есть
                        locationsState.friendLocations.find { it.ownerId == location.ownerId }?.let {
                            it.updateMarkerFlag = true
                            it.updateAvatar = true
                        }
                    }
                }
            }
        })
    }
}