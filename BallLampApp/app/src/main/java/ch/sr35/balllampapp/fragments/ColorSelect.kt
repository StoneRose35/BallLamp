package ch.sr35.balllampapp.fragments

import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.EditText
import android.widget.SeekBar
import android.widget.TextView
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red
import ch.sr35.balllampapp.*
import ch.sr35.balllampapp.backend.LampSelectorData
import ch.sr35.balllampapp.backend.SimpleIntColor
import android.content.DialogInterface
import androidx.appcompat.app.AlertDialog

import ch.sr35.balllampapp.MainActivity





class ColorSelect : Fragment(R.layout.fragment_color_select) {


    private var lbl: TextView? = null
    var connectionState: TextView? = null
    var mainClr: SimpleIntColor = SimpleIntColor(0,0,0)
    var lampBallSelectorUpper: LampSelectorView? = null
    var lampBallSelectorLower: LampSelectorView? = null

    var serialLogger: TextView? = null
    var durationField: EditText? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        val ldLower = savedInstanceState?.getParcelable<LampSelectorData>("lampLowerData")
        val ldUpper = savedInstanceState?.getParcelable<LampSelectorData>("lampUpperData")
        if (ldLower != null)
        {
            lampBallSelectorLower?.lampData=ldLower
        }
        if (ldUpper != null)
        {
            lampBallSelectorUpper?.lampData=ldUpper
        }
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        view.findViewById<SeekBar>(R.id.amountRed).setOnSeekBarChangeListener(
            RGBSeekBarChangeListener(this, COLOR_RED)
        )
        view.findViewById<SeekBar>(R.id.amountGreen).setOnSeekBarChangeListener(
            RGBSeekBarChangeListener(this, COLOR_GREEN)
        )
        view.findViewById<SeekBar>(R.id.amountBlue).setOnSeekBarChangeListener(
            RGBSeekBarChangeListener(this, COLOR_BLUE)
        )

        lbl = view.findViewById(R.id.textViewLblLower)
        connectionState = view.findViewById(R.id.textViewConnectionState)
        serialLogger = view.findViewById(R.id.serialOut)
        lampBallSelectorUpper = view.findViewById(R.id.lampSelectorUpper)
        lampBallSelectorLower =view. findViewById(R.id.lampSelectorLower)
        var durField = view.findViewById<EditText>(R.id.editTextDuration)

        lampBallSelectorUpper?.mappingTable = resources.getIntArray(R.array.lampMappingUpper)
        lampBallSelectorLower?.mappingTable = resources.getIntArray(R.array.lampMappingLower)

        labelConnectButton()

        view.findViewById<Button>(R.id.btnConnect)?.setOnClickListener {
            (activity as MainActivity).initConnection()
        }

        view.findViewById<Button>(R.id.add_to_animation).setOnClickListener {
            val doubleDuration = -1.0
            var txtVal = durField.text
            try {
                val doubleDuration = txtVal.toString().toDouble()
            } catch (e: NumberFormatException) {
                val alertDialog: AlertDialog? =
                    context?.let { it1 -> AlertDialog.Builder(it1).create() }
                alertDialog?.setTitle(resources.getString(R.string.napa_alert_duration_title))
                alertDialog?.setMessage(resources.getString(R.string.napa_alert_duration_message))
                alertDialog?.setButton(
                    AlertDialog.BUTTON_NEGATIVE, resources.getString(R.string.napa_alert_duration_ok)
                ) { dialog, _ -> dialog.cancel() }
                alertDialog?.show()
            }
            if (doubleDuration >= 0.0) {
                for (la in (activity as MainActivity).alFragment.animation.lampAnimations.withIndex()) {

                    if (la.index < 10 && lampBallSelectorUpper != null) {
                        var newstep = Step(
                            lampBallSelectorUpper!!.triangleColors[la.index].clone(),
                            (doubleDuration * 30).toLong(),
                            InterpolationType.LINEAR
                        )
                        la.value.steps.add(newstep)
                    } else {
                        var newstep = Step(
                            lampBallSelectorLower!!.triangleColors[la.index - 10].clone(),
                            (doubleDuration * 30).toLong(),
                            InterpolationType.LINEAR
                        )
                        la.value.steps.add(newstep)
                    }
                }
            }

        }

        initButtons()

        val ld_lower = savedInstanceState?.getParcelable<LampSelectorData>("lampLowerData")
        val ld_upper = savedInstanceState?.getParcelable<LampSelectorData>("lampUpperData")
        if (ld_lower != null)
        {
            lampBallSelectorLower?.lampData=ld_lower
        }
        if (ld_upper != null)
        {
            lampBallSelectorUpper?.lampData=ld_upper
        }

    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)

        outState.putParcelable("lampLowerData",lampBallSelectorLower?.lampData)
        outState.putParcelable("lampUpperData",lampBallSelectorUpper?.lampData)
    }

    fun initButtons()
    {
        view?.findViewById<Button>(R.id.buttonRed)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_red)
        }
        view?.findViewById<Button>(R.id.buttonGreen)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_green)
        }
        view?.findViewById<Button>(R.id.buttonBlue)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_blue)
        }
        view?.findViewById<Button>(R.id.buttonWarmWhite)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_war_white)
        }
        view?.findViewById<Button>(R.id.buttonColdWhite)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_cold_white)
        }
        view?.findViewById<Button>(R.id.buttonBrightWhite)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_bright)
        }
        view?.findViewById<Button>(R.id.buttonOff)?.setOnClickListener {
            setColorOnSelectorAndLamp(R.color.lamp_off)
        }

    }

    private fun setColorOnSelectorAndLamp( resourceIdClr: Int)
    {
        val clr = (activity as MainActivity).getColor(resourceIdClr)
        val siclr = SimpleIntColor(clr.red,clr.green,clr.blue)
        (activity as MainActivity).sendString("RGB(${clr.red},${clr.green},${clr.blue},0-19)\r")
        lampBallSelectorLower?.setColorForAll(siclr)
        lampBallSelectorUpper?.setColorForAll(siclr)
    }

    private fun labelConnectButton()
    {
        var btnConnect: Button? = view?.findViewById(R.id.btnConnect)
        if ((activity as MainActivity).btSocket?.isConnected != true)
        {
            btnConnect?.text = getString(R.string.connect_text)
        }
        else
        {
            btnConnect?.text = getString(R.string.disconnect_text)
        }
    }

    override fun onCreateView(
        inflater: LayoutInflater, container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        // Inflate the layout for this fragment
        return inflater.inflate(R.layout.fragment_color_select, container, false)
    }


}