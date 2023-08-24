package ch.sr35.balllampapp.fragments

import android.os.Bundle
import android.view.View
import android.widget.Button
import android.widget.TextView
import androidx.appcompat.widget.SwitchCompat
import androidx.fragment.app.Fragment
import androidx.fragment.app.setFragmentResultListener
import androidx.recyclerview.widget.ItemTouchHelper
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.*
import ch.sr35.balllampapp.backend.*
import java.io.File
import java.io.FileOutputStream
import java.io.ObjectOutputStream


class AnimationList: Fragment(R.layout.fragment_animation_list) {

    var animation: Animation = Animation(ArrayList())
    var fileName: String? = null

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


        val af = (activity as MainActivity).csFragment.animationFrame //frameViewModel.animationFrame.value
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
        val recyclerView: RecyclerView = view.findViewById(R.id.recycleViewerAnimation)
        recyclerView.adapter = animationListAdapter


        val touchHelperCallback = object: ItemTouchHelper.SimpleCallback(
            ItemTouchHelper.UP.or(ItemTouchHelper.DOWN),
            ItemTouchHelper.RIGHT.or(ItemTouchHelper.LEFT)){
            override fun onMove(
                recyclerView: RecyclerView,
                viewHolder: RecyclerView.ViewHolder,
                target: RecyclerView.ViewHolder
            ): Boolean {
                val p1 = viewHolder.adapterPosition
                val p2 = target.adapterPosition
                for (la in animation.lampAnimations)
                {
                    val swap = la.steps[p2]
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
                        mainAct.csFragment.animationFrame?.lampDataLower = (viewHolder as AnimationListAdapter.ViewHolder).lampSelectorLower.lampData
                        mainAct.csFragment.animationFrame?.lampdataUpper = viewHolder.lampSelectorUpper.lampData
                        mainAct.csFragment.animationFrame?.duration = viewHolder.duration
                        mainAct.csFragment.animationFrame?.editedStep = viewHolder.adapterPosition
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

        val saveButton = view.findViewById<Button>(R.id.saveAnimation)
        saveButton.setOnClickListener {
            val nameDlg = GetNameDialog(animation.name,fileName)
            nameDlg.show(requireActivity().supportFragmentManager,"GetName")
        }
        setDurationAndSize()
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setFragmentResultListener("fragment_get_name_ok")
        {
            _,bundle ->
            var file_idx = 0
            val fname = bundle.getString("fileName")
            animation.name = bundle.getString("animationName")
            if (activity != null) {
                val fDir = activity!!.filesDir
                val newAnimationFile: File
                val savedAnimationDao: SavedAnimationDao
                if (fname == null) {
                    fDir.listFiles()?.iterator()?.forEach {
                        if (it.isFile && it.name.endsWith("anm")) {
                            file_idx = Integer.parseInt(it.name.split(".")[0])

                        }
                    }
                    file_idx += 1

                    savedAnimationDao = SavedAnimationDao(
                        animation.name,
                        fDir.absolutePath + File.separator + file_idx.toString() + ".anm",
                        animation
                    )
                    newAnimationFile = File(savedAnimationDao.fileName)
                }
                else
                {
                    savedAnimationDao = SavedAnimationDao(
                        animation.name,
                        fname,
                        animation
                    )
                    newAnimationFile = File(savedAnimationDao.fileName)
                    if (fDir.listFiles()?.any { f -> f.name==newAnimationFile.name } == true)
                    {
                        newAnimationFile.delete()
                    }
                }
                newAnimationFile.createNewFile()
                val fos = FileOutputStream(newAnimationFile)
                val oos = ObjectOutputStream(fos)
                try {
                    oos.writeObject(savedAnimationDao)
                } catch (e: Exception)
                {
                    e.printStackTrace()
                }
                oos.close()
            }

        }
    }

    private fun setDurationAndSize(){
        val textViewAnimDescr = view?.findViewById<TextView>(R.id.textViewAnimationDescription)
        val durationString = String.format("%.2f",animation.getTotalDurationInSeconds())
        val byteSizeString = String.format("%d",animation.getByteSize())
        val txt = resources.getString(R.string.tv_anim_descr,durationString,byteSizeString)
        textViewAnimDescr?.text = txt
    }

}