package ch.sr35.balllampapp.fragments

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.View
import androidx.recyclerview.widget.ItemTouchHelper
import androidx.recyclerview.widget.RecyclerView
import ch.sr35.balllampapp.R
import ch.sr35.balllampapp.SavedAnimationAdapter
import ch.sr35.balllampapp.backend.SavedAnimationDao
import java.io.FileInputStream
import java.io.IOException
import java.io.InvalidClassException
import java.io.ObjectInputStream
import java.io.OptionalDataException
import java.io.StreamCorruptedException


/**
 * A simple [Fragment] subclass.
 * Use the [SavedAnimations.newInstance] factory method to
 * create an instance of this fragment.
 */
class SavedAnimations : Fragment(R.layout.fragment_saved_animations) {
    // TODO: Rename and change types of parameters
    var savedAnimations: ArrayList<SavedAnimationDao> = ArrayList<SavedAnimationDao>()

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        var fDir = activity!!.filesDir
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
                } catch (e: ClassNotFoundException)
                {

                } catch (e1: InvalidClassException)
                {

                } catch (e2: StreamCorruptedException)
                {

                } catch (e3: OptionalDataException)
                {

                } catch (e4: IOException)
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
            ItemTouchHelper.START)
        {
            override fun onMove(
                recyclerView: RecyclerView,
                viewHolder: RecyclerView.ViewHolder,
                target: RecyclerView.ViewHolder
            ): Boolean {
                TODO("Not yet implemented")
            }

            override fun onSwiped(viewHolder: RecyclerView.ViewHolder, direction: Int) {

            }

        }
    }

}