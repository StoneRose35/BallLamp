package ch.sr35.balllampapp

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothSocket
import android.os.Bundle
import androidx.fragment.app.Fragment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.Button
import android.widget.SeekBar
import android.widget.TextView
import androidx.core.graphics.blue
import androidx.core.graphics.green
import androidx.core.graphics.red


class colorSelect : Fragment(R.layout.fragment_color_select) {


    private var lbl: TextView? = null
    var connectionState: TextView? = null
    var mainClr: SimpleIntColor = SimpleIntColor(0,0,0)
    var lampBallSelectorUpper: LampSelectorView? = null
    var lampBallSelectorLower: LampSelectorView? = null

    var serialLogger: TextView? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
    }

    override fun onViewCreated(view: View, savedInstanceState: Bundle?) {
        super.onViewCreated(view, savedInstanceState)

        view.findViewById<SeekBar>(R.id.amountRed).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_RED))
        view.findViewById<SeekBar>(R.id.amountGreen).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_GREEN))
        view.findViewById<SeekBar>(R.id.amountBlue).setOnSeekBarChangeListener(RGBSeekBarChangeListener(this,COLOR_BLUE))

        lbl = view.findViewById(R.id.textViewLblLower)
        connectionState = view.findViewById(R.id.textViewConnectionState)
        serialLogger = view.findViewById(R.id.serialOut)
        lampBallSelectorUpper = view.findViewById(R.id.lampSelectorUpper)
        lampBallSelectorLower =view. findViewById(R.id.lampSelectorLower)

        lampBallSelectorUpper?.mappingTable = resources.getIntArray(R.array.lampMappingUpper)
        lampBallSelectorLower?.mappingTable = resources.getIntArray(R.array.lampMappingLower)

        labelConnectButton()

        view.findViewById<Button>(R.id.btnConnect)?.setOnClickListener {
            (activity as MainActivity).initConnection()
        }

        initButtons()
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