package com.artem.tusaandroid

import android.opengl.EGL14
import android.opengl.EGLConfig
import android.opengl.EGLContext
import android.opengl.EGLDisplay
import android.opengl.EGLSurface
import java.util.concurrent.atomic.AtomicBoolean

class SecondaryGLThread(
    private val sharedContext: EGLContext, // Передаем существующий контекст
    private val width: Int,
    private val height: Int
) {
    private var eglDisplay: EGLDisplay = EGL14.EGL_NO_DISPLAY
    private var eglContext: EGLContext = EGL14.EGL_NO_CONTEXT
    private var eglSurface: EGLSurface = EGL14.EGL_NO_SURFACE
    private val running = AtomicBoolean(false)
    private var thread: Thread? = null

    private val configAttributes = intArrayOf(
        EGL14.EGL_RENDERABLE_TYPE, EGL14.EGL_OPENGL_ES2_BIT,
        EGL14.EGL_RED_SIZE, 8,
        EGL14.EGL_GREEN_SIZE, 8,
        EGL14.EGL_BLUE_SIZE, 8,
        EGL14.EGL_ALPHA_SIZE, 8,
        EGL14.EGL_DEPTH_SIZE, 16,
        EGL14.EGL_STENCIL_SIZE, 8,
        EGL14.EGL_NONE
    )

    private val contextAttributes = intArrayOf(
        EGL14.EGL_CONTEXT_CLIENT_VERSION, 2,
        EGL14.EGL_NONE
    )

    init {
        setupEGL()
        startRenderThread()
    }

    private fun setupEGL() {
        // Получаем EGL дисплей
        eglDisplay = EGL14.eglGetDisplay(EGL14.EGL_DEFAULT_DISPLAY)
        if (eglDisplay == EGL14.EGL_NO_DISPLAY) {
            throw RuntimeException("Unable to get EGL display")
        }

        // Инициализируем EGL
        val version = IntArray(2)
        if (!EGL14.eglInitialize(eglDisplay, version, 0, version, 1)) {
            throw RuntimeException("Unable to initialize EGL")
        }

        // Выбираем конфигурацию
        val configs = arrayOfNulls<EGLConfig>(1)
        val numConfigs = IntArray(1)
        EGL14.eglChooseConfig(
            eglDisplay, configAttributes, 0,
            configs, 0, 1, numConfigs, 0
        )

        // Создаем off-screen поверхность
        val surfaceAttributes = intArrayOf(
            EGL14.EGL_WIDTH, width,
            EGL14.EGL_HEIGHT, height,
            EGL14.EGL_NONE
        )
        eglSurface = EGL14.eglCreatePbufferSurface(
            eglDisplay, configs[0], surfaceAttributes, 0
        )

        // Создаем новый контекст, унаследованный от существующего
        eglContext = EGL14.eglCreateContext(
            eglDisplay, configs[0],
            sharedContext, // Используем переданный контекст для шеринга
            contextAttributes, 0
        )

        if (eglContext == EGL14.EGL_NO_CONTEXT) {
            throw RuntimeException("Unable to create shared EGL context")
        }
    }

    private fun startRenderThread() {
        running.set(true)
        thread = Thread {
            // Устанавливаем контекст для этого потока
            if (!EGL14.eglMakeCurrent(eglDisplay, eglSurface, eglSurface, eglContext)) {
                throw RuntimeException("Unable to make EGL context current")
            }

            NativeLibrary.initParallel()

            while (running.get()) {
                try {
                    render()
                    Thread.sleep(300)
                } catch (e: Exception) {
                    e.printStackTrace()
                }
            }
        }
        thread?.start()
    }

    private fun render() {
        NativeLibrary.renderParallel()
    }

    fun destroy() {
        running.set(false)
        thread?.join()

        EGL14.eglMakeCurrent(
            eglDisplay, EGL14.EGL_NO_SURFACE,
            EGL14.EGL_NO_SURFACE, EGL14.EGL_NO_CONTEXT
        )
        EGL14.eglDestroySurface(eglDisplay, eglSurface)
        EGL14.eglDestroyContext(eglDisplay, eglContext)
        EGL14.eglTerminate(eglDisplay)
    }
}

// Пример использования с GLSurfaceView
//class MainActivity : AppCompatActivity() {
//    private lateinit var glSurfaceView: GLSurfaceView
//    private lateinit var secondaryGLThread: SecondaryGLThread
//    private var sharedEglContext: EGLContext? = null
//
//    override fun onCreate(savedInstanceState: Bundle?) {
//        super.onCreate(savedInstanceState)
//
//        glSurfaceView = GLSurfaceView(this)
//        setContentView(glSurfaceView)
//
//        // Настраиваем основной GLSurfaceView
//        glSurfaceView.setEGLContextClientVersion(2)
//        glSurfaceView.setRenderer(object : GLSurfaceView.Renderer {
//            override fun onSurfaceCreated(gl: GL10?, config: EGLConfig?) {
//                // Получаем текущий контекст для передачи во второй поток
//                sharedEglContext = EGL14.eglGetCurrentContext()
//
//                // Создаем второй поток с общим контекстом
//                sharedEglContext?.let {
//                    secondaryGLThread = SecondaryGLThread(this@MainActivity, it)
//                }
//            }
//
//            override fun onDrawFrame(gl: GL10?) {
//                GLES20.glClear(GLES20.GL_COLOR_BUFFER_BIT)
//            }
//
//            override fun onSurfaceChanged(gl: GL10?, width: Int, height: Int) {
//                GLES20.glViewport(0, 0, width, height)
//            }
//        })
//    }
//
//    override fun onDestroy() {
//        super.onDestroy()
//        secondaryGLThread.destroy()
//    }
//
//    override fun onPause() {
//        super.onPause()
//        glSurfaceView.onPause()
//    }
//
//    override fun onResume() {
//        super.onResume()
//        glSurfaceView.onResume()
//    }
//}