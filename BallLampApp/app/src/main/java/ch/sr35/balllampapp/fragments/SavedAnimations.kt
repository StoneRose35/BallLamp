package ch.sr35.balllampapp.fragments

import android.app.AlertDialog
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.View
import androidx.recyclerview.widget.ItemTouchHelper
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.MainActivity
import ch.sr35.balllampapp.R
import ch.sr35.balllampapp.SavedAnimationAdapter
import ch.sr35.balllampapp.backend.SavedAnimationDao
import java.io.File
import java.io.FileInputStream
import java.io.IOException
import java.io.InvalidClassException
import java.io.ObjectInputStream
import java.io.OptionalDataException
import java.io.StreamCorruptedException


/**
 * A simple [Fragment] subclass.
 */
class SavedAnimations : Fragment(R.layout.fragment_saved_animations) {
    var savedAnimations: ArrayList<SavedAnimationDao> = ArrayList<SavedAnimationDao>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        val fDir = activity!!.filesDir
        savedAnimations.clear()
        fDir.listFiles()?.iterator()?.forEach {
            if (it.isFile && it.name.endsWith("anm")) {
                val fis = FileInputStream(it)
                val ois = ObjectInputStream(fis)
                try {
                    val readbackObject = ois.readObject()
                    if (readbackObject is SavedAnimationDao)
                    {
                        savedAnimations.add(readbackObject)
                    }
                } catch (_: ClassNotFoundException)
                {

                } catch (_: InvalidClassException)
                {

                } catch (_: StreamCorruptedException)
                {

                } catch (_: OptionalDataException)
                {

                } catch (_: IOException)
                {

                }

            }
        }

    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        val savedAnimationAdapter = SavedAnimationAdapter(savedAnimations)
        val recyclerView: RecyclerView = view.findViewById(R.id.recycleViewSavedAnimation)
        recyclerView.adapter = savedAnimationAdapter

        val touchHelperCallback = object: ItemTouchHelper.SimpleCallback(
            ItemTouchHelper.UP.or(ItemTouchHelper.DOWN),
            ItemTouchHelper.RIGHT.or(ItemTouchHelper.LEFT)){
            override fun onMove(
                recyclerView: RecyclerView,
                viewHolder: RecyclerView.ViewHolder,
                target: RecyclerView.ViewHolder
            ): Boolean {
                return false
            }

            override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {
                when (direction)
                {
                    ItemTouchHelper.RIGHT -> {
                        val alertDlgBuilder =  AlertDialog.Builder((activity as MainActivity))
                            .setMessage("really delete")
                            .setPositiveButton("Yes") { _, _ ->
                                val fileToDelete = File(savedAnimations[viewHolder.adapterPosition].fileName)
                                fileToDelete.delete()
                                savedAnimations.removeAt(viewHolder.adapterPosition)
                                savedAnimationAdapter.notifyItemRemoved(viewHolder.adapterPosition)
                                
                            }
                            .setNegativeButton("No") {_, _ -> savedAnimationAdapter.notifyItemChanged(viewHolder.adapterPosition)}
                        val alertDlg = alertDlgBuilder.create()
                        alertDlg.show()
                    }
                }
            }

        }
        val touchHelper = ItemTouchHelper(touchHelperCallback)
        touchHelper.attachToRecyclerView(recyclerView)
    }

}