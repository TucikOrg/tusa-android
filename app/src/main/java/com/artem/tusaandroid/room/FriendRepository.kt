package com.artem.tusaandroid.room

import kotlinx.coroutines.flow.Flow

class FriendRepository(private val friendDao: FriendDao) {
    val allFriends: Flow<List<FriendRoomEntity>> = friendDao.getAllFriends()

    suspend fun insert(user: FriendRoomEntity) {
        friendDao.insert(user)
    }

    suspend fun deleteByPhone(phone: String) {
        friendDao.deleteById(phone)
    }
}