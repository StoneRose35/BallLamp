package ch.sr35.balllampapp.fragments

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.View
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

    }

}