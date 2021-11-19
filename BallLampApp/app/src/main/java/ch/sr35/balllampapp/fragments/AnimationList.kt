package ch.sr35.balllampapp.fragments

import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.widget.SwitchCompat
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.*
import ch.sr35.balllampapp.backend.CommandDispatcher
import ch.sr35.balllampapp.backend.SimpleIntColor

class AnimationList: Fragment(R.layout.fragment_animation_list) {

    var animation: Animation = Animation(ArrayList<LampAnimation>())

    init {
        for (i in 0..19) {
            var steps = ArrayList<Step>()
            animation.lampAnimations.add(LampAnimation(i.toByte(), steps, true))
        }
    }


    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        animation.mappingUpper = resources.getIntArray(R.array.lampMappingUpper)
        animation.mappingLower = resources.getIntArray(R.array.lampMappingLower)

        val animationListAdapter = AnimationListAdapter(animation)

        val recyclerView: RecyclerView = view.findViewById(R.id.recycleViewer)
        recyclerView.adapter = animationListAdapter

        val switchAnimationOn = view.findViewById<SwitchCompat>(R.id.switchAnimationOn)
        switchAnimationOn.setOnCheckedChangeListener { buttonView, isChecked ->
            var cmds = ArrayList<String>();
            if (isChecked)
            {

                cmds.add("STOP\r")

                val commands = animation.toCommandList()

                cmds.addAll(commands)
                cmds.add("START\r")
                val cmdDispatcher = CommandDispatcher(cmds, (activity as MainActivity))
                cmdDispatcher.start()
            }
            else
            {
                cmds.add("STOP\r")
                val cmdDispatcher = CommandDispatcher(cmds, (activity as MainActivity))
                cmdDispatcher.start()
            }
        }
        var textViewAnimDescr = view.findViewById<TextView>(R.id.textViewAnimationDescription)
        val durationString = String.format("%.2f",animation.getTotalDurationInSeconds())
        val byteSizeString = String.format("%d",animation.getByteSize())
        var txt = resources.getString(R.string.tv_anim_descr,durationString,byteSizeString)
        textViewAnimDescr.text = txt
    }
}