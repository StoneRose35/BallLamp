package ch.sr35.balllampapp

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import androidx.recyclerview.widget.RecyclerView

class AnimationList: AppCompatActivity() {

    private var animation: Animation

    init {
        animation = Animation(ArrayList<LampAnimation>())
        for (i in 0..19) {
            var steps = ArrayList<Step>()
            for (t in 0..3) {
                steps.add(Step(SimpleIntColor.randomColor(), 42, InterpolationType.LINEAR))
            }
            animation.lampAnimations.add(LampAnimation(i.toByte(), steps, true))
        }
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_animation_list)
        supportActionBar?.hide()
        val animationListAdapter = AnimationListAdapter(animation)

        val recyclerView: RecyclerView = findViewById(R.id.recycleViewer)
        recyclerView.adapter = animationListAdapter

    }
}