plugins {
    alias(libs.plugins.android.application)
    alias(libs.plugins.jetbrains.kotlin.android)
    alias(libs.plugins.compose.compiler)
    id("dagger.hilt.android.plugin")
    id("com.google.devtools.ksp")
    kotlin("plugin.serialization") version "2.0.21"
    id("com.google.gms.google-services")
}

android {
    namespace = "com.artem.tusaandroid"
    compileSdk = 35

    defaultConfig {
        applicationId = "com.artem.tusaandroid"
        minSdk = 30
        targetSdk = 35
        versionCode = 8
        versionName = "1.0"

        testInstrumentationRunner = "androidx.test.runner.AndroidJUnitRunner"
        vectorDrawables {
            useSupportLibrary = true
        }
    }

    buildFeatures {
        buildConfig = true
    }
    buildTypes {
        release {
            isMinifyEnabled = true
            isShrinkResources = true
            proguardFiles(
                getDefaultProguardFile("proguard-android-optimize.txt"),
                "proguard-rules.pro"
            )
            buildConfigField("String", "SERVICE_URL", "\"https://tucik.fun\"")
            buildConfigField("String", "SOCKET_URL", "\"wss://tucik.fun/stream\"")
            buildConfigField("String", "MAP_URL", "\"https://tucik.fun/api/v1/tiles\"")
            signingConfig = signingConfigs.getByName("debug")
            buildConfigField("Boolean", "MAP_DEBUG", "false")
        }
        debug {
            buildConfigField("String", "SERVICE_URL", "\"http://192.168.1.100:8080\"")
            buildConfigField("String", "SOCKET_URL", "\"ws://192.168.1.100:8080/stream\"")
            buildConfigField("String", "MAP_URL", "\"http://192.168.1.100:8080/api/v1/tiles\"")
            buildConfigField("Boolean", "MAP_DEBUG", "false")
        }
        create("debugReleaseURLS") {
            isDebuggable = true
            buildConfigField("String", "SERVICE_URL", "\"https://tucik.fun\"")
            buildConfigField("String", "SOCKET_URL", "\"wss://tucik.fun/stream\"")
            buildConfigField("String", "MAP_URL", "\"https://tucik.fun/api/v1/tiles\"")
            buildConfigField("Boolean", "MAP_DEBUG", "false")
            signingConfig = signingConfigs.getByName("debug")
        }
    }
    compileOptions {
        sourceCompatibility = JavaVersion.VERSION_1_8
        targetCompatibility = JavaVersion.VERSION_1_8
    }
    externalNativeBuild {
        cmake {
            path = file("src/main/cpp/CMakeLists.txt")
            version = "3.22.1"
        }
    }
    kotlinOptions {
        jvmTarget = "1.8"
    }
    buildFeatures {
        compose = true
    }
    composeOptions {
        kotlinCompilerExtensionVersion = "1.5.1"
    }
    packaging {
        resources {
            excludes += "/META-INF/{AL2.0,LGPL2.1}"
        }
    }
}

dependencies {
    // i add this libraries
    implementation(libs.kotlinx.serialization.json)
    implementation(libs.kotlinx.serialization.cbor)
    implementation(libs.coil.compose)
    implementation(libs.kotlinx.coroutines.android)
    implementation(libs.okhttp)
    implementation(libs.play.services.location)
    implementation(libs.accompanist.permissions)
    implementation(libs.core)
    implementation(libs.androidx.work.runtime.ktx)
    implementation(libs.accompanist.systemuicontroller)
    implementation(libs.play.services.fitness)
    implementation(libs.firebase.messaging.ktx)
    implementation(libs.firebase.bom)
    implementation(libs.coil.gif)


    implementation(libs.androidx.room.runtime)
    implementation(libs.androidx.exifinterface)
    ksp(libs.androidx.room.compiler)
    implementation(libs.androidx.room.ktx)

    implementation(libs.hilt.android)
    ksp(libs.hilt.compiler)
    implementation(libs.androidx.hilt.navigation.compose)
    implementation(libs.androidx.hilt.work)
    ksp(libs.androidx.hilt.compiler)

    implementation(libs.moshi.kotlin)
    implementation(libs.moshi.adapters)

    implementation(libs.androidx.core.ktx)
    implementation(libs.androidx.lifecycle.runtime.ktx)
    implementation(libs.androidx.activity.compose)
    implementation(platform(libs.androidx.compose.bom))
    implementation(libs.androidx.ui)
    implementation(libs.androidx.ui.graphics)
    implementation(libs.androidx.ui.tooling.preview)
    implementation(libs.androidx.material3)

    implementation(libs.androidx.credentials)
    implementation(libs.credentials.play.services.auth)
    implementation(libs.googleid)

    testImplementation(libs.junit)
    androidTestImplementation(libs.androidx.junit)
    androidTestImplementation(libs.androidx.espresso.core)
    androidTestImplementation(platform(libs.androidx.compose.bom))
    androidTestImplementation(libs.androidx.ui.test.junit4)
    debugImplementation(libs.androidx.ui.tooling)
    debugImplementation(libs.androidx.ui.test.manifest)
}