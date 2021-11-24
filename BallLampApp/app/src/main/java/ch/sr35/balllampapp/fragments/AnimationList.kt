package ch.sr35.balllampapp.fragments

import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.appcompat.widget.SwitchCompat
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.ItemTouchHelper
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.*
import ch.sr35.balllampapp.backend.CommandDispatcher


class AnimationList: Fragment(R.layout.fragment_animation_list) {

    var animation: Animation = Animation(ArrayList())

    init {
        for (i in 0..19) {
            val steps = ArrayList<Step>()
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

        val touchHelperCallback = object: ItemTouchHelper.SimpleCallback(
                ItemTouchHelper.UP
            .or(ItemTouchHelper.DOWN)
            .or(ItemTouchHelper.LEFT)
            .or(ItemTouchHelper.RIGHT), ItemTouchHelper.RIGHT){
            override fun onMove(
                recyclerView: RecyclerView,
                viewHolder: RecyclerView.ViewHolder,
                target: RecyclerView.ViewHolder
            ): Boolean {
                return false
                //TODO("Not yet implemented")
            }

            override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {
                //TODO("Not yet implemented")
                when(direction)
                {
                    ItemTouchHelper.RIGHT -> {
                        for (la in animation.lampAnimations) {
                            la.steps.removeAt(viewHolder.adapterPosition)
                        }
                        animationListAdapter.notifyItemRemoved(viewHolder.adapterPosition)
                    }
                    ItemTouchHelper.LEFT -> {
                        // move to edit screen, fill duration and colors
                        val mainAct = (activity as MainActivity)
                        mainAct.alInstanceState = mainAct.supportFragmentManager.saveFragmentInstanceState(mainAct.alFragment)
                        mainAct.csFragment.lampBallSelectorUpper?.lampData = (viewHolder as AnimationListAdapter.ViewHolder).lampSelectorUpper.lampData
                        mainAct.csFragment.lampBallSelectorLower?.lampData = viewHolder.lampSelectorLower.lampData
                        mainAct.csFragment.duration = viewHolder.duration
                        mainAct.setFragment(mainAct.csFragment)

                        (activity as MainActivity).supportFragmentManager.beginTransaction()
                    }
                }
            }
        }

        val touchHelper = ItemTouchHelper(touchHelperCallback)
        touchHelper.attachToRecyclerView(recyclerView)

        val switchAnimationOn = view.findViewById<SwitchCompat>(R.id.switchAnimationOn)
        switchAnimationOn.setOnCheckedChangeListener { _, isChecked ->
            val cmds = ArrayList<String>()
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
        val textViewAnimDescr = view.findViewById<TextView>(R.id.textViewAnimationDescription)
        val durationString = String.format("%.2f",animation.getTotalDurationInSeconds())
        val byteSizeString = String.format("%d",animation.getByteSize())
        val txt = resources.getString(R.string.tv_anim_descr,durationString,byteSizeString)
        textViewAnimDescr.text = txt
    }
}