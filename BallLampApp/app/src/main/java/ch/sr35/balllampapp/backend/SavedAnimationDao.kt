package ch.sr35.balllampapp.backend

import java.io.Serializable

class SavedAnimationDao(val animationName: String?,
                        val fileName:String,
                        val animation: Animation?) :Serializable {
}
