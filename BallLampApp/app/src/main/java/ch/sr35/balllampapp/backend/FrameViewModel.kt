package ch.sr35.balllampapp.backend

import androidx.lifecycle.LiveData
import androidx.lifecycle.MutableLiveData
import androidx.lifecycle.ViewModel

class FrameViewModel: ViewModel() {
    private val mutableData= MutableLiveData<AnimationFrame>()
    val animationFrame: LiveData<AnimationFrame> get() = mutableData

    init {
        mutableData.value = AnimationFrame(null,null,null,null)
    }
}