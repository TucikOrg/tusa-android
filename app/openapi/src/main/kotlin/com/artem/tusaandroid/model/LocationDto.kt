/**
 *
 * Please note:
 * This class is auto generated by OpenAPI Generator (https://openapi-generator.tech).
 * Do not edit this file manually.
 *
 */

@file:Suppress(
    "ArrayInDataClass",
    "EnumEntryName",
    "RemoveRedundantQualifierName",
    "UnusedImport"
)

package com.artem.tusaandroid.model


import com.squareup.moshi.Json
import com.squareup.moshi.JsonClass

/**
 * 
 *
 * @param phone 
 * @param latitude 
 * @param longitude 
 */


data class LocationDto (

    @Json(name = "phone")
    val phone: kotlin.String,

    @Json(name = "latitude")
    val latitude: kotlin.Float,

    @Json(name = "longitude")
    val longitude: kotlin.Float

) {


}
