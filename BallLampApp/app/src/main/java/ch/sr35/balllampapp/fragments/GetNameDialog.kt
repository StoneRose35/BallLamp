package ch.sr35.balllampapp.fragments

import android.annotation.SuppressLint
import android.app.AlertDialog
import android.app.Dialog
import android.os.Bundle
import android.widget.Switch
import android.widget.TextView
import androidx.core.os.bundleOf
import androidx.fragment.app.DialogFragment
import androidx.fragment.app.setFragmentResult
import ch.sr35.balllampapp.*

class GetNameDialog(private var animationName: String?, private var fileName: String?): DialogFragment()  {

    @SuppressLint("UseSwitchCompatOrMaterialCode")
    override fun onCreateDialog(savedInstanceState: Bundle?): Dialog {

        return activity?.let {
            val builder = AlertDialog.Builder(it)
            // Get the layout inflater
            val inflater = requireActivity().layoutInflater

            // Inflate and set the layout for the dialog
            // Pass null as the parent view because its going in the dialog layout
            val dlgView = inflater.inflate(R.layout.get_name_layout, null)
            val tv = dlgView.findViewById<TextView>(R.id.getNameEditText)
            val saveAsNewSwitch = dlgView.findViewById<Switch>(R.id.switchSaveAsNew)
            if (animationName != null) {
                tv.text = animationName
            }
            else
            {
                tv.text = getString(R.string.animation_name_default)
            }
            if (fileName == null)
            {
                saveAsNewSwitch.isChecked = true
                saveAsNewSwitch.isEnabled = false
            }
            else
            {
                saveAsNewSwitch.isEnabled = true
            }
            builder.setView(dlgView)
                .setPositiveButton("Ok") { _,_ ->

                    if (!saveAsNewSwitch.isChecked)
                    {
                        setFragmentResult("fragment_get_name_ok", bundleOf("animationName" to tv.text.toString(),"fileName" to fileName))
                    }
                    else
                    {
                        setFragmentResult("fragment_get_name_ok", bundleOf("animationName" to tv.text.toString()))
                    }
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