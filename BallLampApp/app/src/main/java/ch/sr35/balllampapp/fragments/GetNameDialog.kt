package ch.sr35.balllampapp.fragments

import android.app.AlertDialog
import android.app.Dialog
import android.content.Context
import android.os.Bundle
import android.widget.TextView
import androidx.core.os.bundleOf
import androidx.fragment.app.DialogFragment
import androidx.fragment.app.setFragmentResult
import ch.sr35.balllampapp.*
import java.lang.ClassCastException

class GetNameDialog: DialogFragment()  {

    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {

        return activity?.let {
            val builder = AlertDialog.Builder(it)
            // Get the layout inflater
            val inflater = requireActivity().layoutInflater;

            // Inflate and set the layout for the dialog
            // Pass null as the parent view because its going in the dialog layout
            val dlgView = inflater.inflate(R.layout.get_name_layout, null)
            builder.setView(dlgView)
                .setPositiveButton("Ok") { _,_ ->
                    val tv = dlgView.findViewById<TextView>(R.id.getNameEditText)
                    setFragmentResult("fragment_get_name_ok", bundleOf("animationName" to tv.text.toString()))
                }
                .setNegativeButton("Cancel")
                {
                    _,_ ->
                }
            // Add action buttons

            builder.create()
        } ?: throw IllegalStateException("Activity cannot be null")
    }



}