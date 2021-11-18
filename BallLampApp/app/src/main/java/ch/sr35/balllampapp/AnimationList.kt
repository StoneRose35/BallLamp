package ch.sr35.balllampapp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.view.View
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.RecyclerView

class AnimationList: Fragment(R.layout.activity_animation_list) {

    private var animation: Animation = Animation(ArrayList<LampAnimation>())

    init {
        for (i in 0..19) {
            var steps = ArrayList<Step>()
            for (t in 0..3) {
                steps.add(Step(SimpleIntColor.randomColor(), 42, InterpolationType.LINEAR))
            }
            animation.lampAnimations.add(LampAnimation(i.toByte(), steps, true))
        }
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val animationListAdapter = AnimationListAdapter(animation)

        val recyclerView: RecyclerView = view.findViewById(R.id.recycleViewer)
        recyclerView.adapter = animationListAdapter
    }
}