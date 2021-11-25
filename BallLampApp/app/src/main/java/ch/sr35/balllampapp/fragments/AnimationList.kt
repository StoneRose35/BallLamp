package ch.sr35.balllampapp.fragments

import android.os.Bundle
import android.view.View
import android.widget.TextView
import androidx.appcompat.widget.SwitchCompat
import androidx.fragment.app.Fragment
import androidx.fragment.app.activityViewModels
import androidx.recyclerview.widget.ItemTouchHelper
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.*
import ch.sr35.balllampapp.backend.CommandDispatcher
import ch.sr35.balllampapp.backend.FrameViewModel


class AnimationList: Fragment(R.layout.fragment_animation_list) {

    var animation: Animation = Animation(ArrayList())
    val frameViewModel: FrameViewModel by activityViewModels()

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


        val af = frameViewModel.animationFrame.value
            if (af?.editedStep != null) {
                for (la in animation.lampAnimations.withIndex())
                {
                    if (la.index < 10) {
                        if (af.lampdataUpper?.colors != null) {
                            la.value.steps[af.editedStep!!].color =
                                af.lampdataUpper?.colors!![la.index]
                        }
                    }
                    else
                    {
                        if (af.lampDataLower?.colors != null) {
                            la.value.steps[af.editedStep!!].color =
                                af.lampDataLower?.colors!![la.index-10]
                        }
                    }
                    la.value.steps[af.editedStep!!].duration =
                        (af.duration?.times(30.0))?.toLong() ?: 0
                }

            }

        val animationListAdapter = AnimationListAdapter(animation)
        val recyclerView: RecyclerView = view.findViewById(R.id.recycleViewer)
        recyclerView.adapter = animationListAdapter


        val touchHelperCallback = object: ItemTouchHelper.SimpleCallback(
            ItemTouchHelper.UP.or(ItemTouchHelper.DOWN),
            ItemTouchHelper.RIGHT.or(ItemTouchHelper.LEFT)){
            override fun onMove(
                recyclerView: RecyclerView,
                viewHolder: RecyclerView.ViewHolder,
                target: RecyclerView.ViewHolder
            ): Boolean {
                var p1 = viewHolder.adapterPosition
                var p2 = target.adapterPosition
                for (la in animation.lampAnimations)
                {
                    var swap = la.steps[p2]
                    la.steps[p2] = la.steps[p1]
                    la.steps[p1] = swap
                }
                recyclerView.adapter?.notifyItemMoved(p1,p2)
                return true
            }

            override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {
                when(direction)
                {
                    ItemTouchHelper.RIGHT -> {
                        for (la in animation.lampAnimations) {
                            la.steps.removeAt(viewHolder.adapterPosition)
                        }
                        animationListAdapter.notifyItemRemoved(viewHolder.adapterPosition)
                        setDurationAndSize()
                    }
                    ItemTouchHelper.LEFT -> {
                        // move to edit screen, fill duration and colors
                        val mainAct = (activity as MainActivity)
                        frameViewModel.animationFrame.value?.lampDataLower = (viewHolder as AnimationListAdapter.ViewHolder).lampSelectorLower.lampData
                        frameViewModel.animationFrame.value?.lampdataUpper = viewHolder.lampSelectorUpper.lampData
                        frameViewModel.animationFrame.value?.duration = viewHolder.duration
                        frameViewModel.animationFrame.value?.editedStep = viewHolder.adapterPosition
                        mainAct.setFragment(mainAct.csFragment)
                    }
                }
            }

            override fun getMovementFlags(
                recyclerView: RecyclerView,
                viewHolder: RecyclerView.ViewHolder
            ): Int {
                return makeMovementFlags(ItemTouchHelper.UP.or(ItemTouchHelper.DOWN),
                    ItemTouchHelper.LEFT.or(ItemTouchHelper.RIGHT))
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
        setDurationAndSize()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    private fun setDurationAndSize(){
        val textViewAnimDescr = view?.findViewById<TextView>(R.id.textViewAnimationDescription)
        val durationString = String.format("%.2f",animation.getTotalDurationInSeconds())
        val byteSizeString = String.format("%d",animation.getByteSize())
        val txt = resources.getString(R.string.tv_anim_descr,durationString,byteSizeString)
        textViewAnimDescr?.text = txt
    }
}