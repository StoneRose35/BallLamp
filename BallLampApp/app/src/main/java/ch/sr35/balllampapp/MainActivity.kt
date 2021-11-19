package ch.sr35.balllampapp

import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.graphics.Color
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.PersistableBundle
import android.widget.*
import androidx.fragment.app.Fragment
import androidx.fragment.app.FragmentManager
import androidx.fragment.app.FragmentTransaction
import ch.sr35.balllampapp.fragments.AnimationList
import ch.sr35.balllampapp.fragments.ColorSelect
import com.google.android.material.bottomnavigation.BottomNavigationView
import java.io.IOException
import java.io.InputStream


const val COLOR_RED = 0
const val COLOR_GREEN = 1
const val COLOR_BLUE = 2

class MainActivity : AppCompatActivity() {

    var connectionState: TextView? = null
    var btSocket: BluetoothSocket? = null
    var btAdapter: BluetoothAdapter? = null
    var connectionInitActive: Boolean = true
    var btReceiverThread: BluetoothReceiverThread? = null
    var serialLogger: TextView? = null

    var csInstanceState: Fragment.SavedState? = null
    var alInstanceState: Fragment.SavedState? = null


    override fun onCreate(savedInstanceState: Bundle?) {
        val btMan: BluetoothManager = applicationContext.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        btAdapter = btMan.adapter
        connectionInitActive = true

        super.onCreate(savedInstanceState)

        val csFragment = ColorSelect() // supportFragmentManager.findFragmentById(R.id.frag_color_select)
        val alFragment= AnimationList() // supportFragmentManager.findFragmentById(R.id.frag_animation_list)



        setContentView(R.layout.activity_main)

        var bottomNavigationView = findViewById<BottomNavigationView>(R.id.navigator)
        bottomNavigationView.setOnNavigationItemSelectedListener {
            when(it.itemId){
                R.id.nav_color_select-> {
                    if (supportFragmentManager.fragments.contains(alFragment)) {
                        alInstanceState = supportFragmentManager.saveFragmentInstanceState(alFragment)
                    }
                    csFragment.setInitialSavedState(csInstanceState)
                    setFragment(csFragment)
                }

                R.id.nav_animation_view-> {
                    if (supportFragmentManager.fragments.contains(csFragment)) {
                        csInstanceState = supportFragmentManager.saveFragmentInstanceState(csFragment)
                    }
                    alFragment.setInitialSavedState(alInstanceState)
                    setFragment(alFragment)

                }
            }
            true
        }

        val btReceiver = BTReceiver(this)
        registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_FOUND))
        registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
        registerReceiver(btReceiver, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))


    }


    fun setFragment(fragment: Fragment)
    {
        supportFragmentManager.beginTransaction().apply {
            replace(R.id.main_frame,fragment)
            commit()
        }
    }


    override fun onPause() {
        super.onPause()
        btReceiverThread?.isRunning=false
    }

    override fun onResume() {
        super.onResume()
        btReceiverThread?.isRunning=true
        if (btReceiverThread?.isAlive != true)
        {
            btReceiverThread?.start()
        }

    }

    fun initConnection()
    {
        if (btSocket?.isConnected != true) {

            if (connectionInitActive) {
                if (btAdapter?.state == BluetoothAdapter.STATE_OFF) {
                    val btOnIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                    connectionInitActive = false
                    connectionState?.text = getString(R.string.bt_switch_on)
                    startActivity(btOnIntent)
                } else {
                    val btDevice =
                        btAdapter?.bondedDevices?.firstOrNull { e -> e.name == DEVICE_NAME }
                    if (btDevice == null) {
                        connectionState?.text = getString(R.string.bt_discovery)
                        btAdapter?.startDiscovery()
                        connectionInitActive = false
                    } else {

                        btSocket = btDevice.createRfcommSocketToServiceRecord(appUuid)
                        try {
                            btSocket?.connect() // is blocking, so the connection is established on the next line
                            connectionState?.text = getString(R.string.bt_connected)

                            btReceiverThread =
                                BluetoothReceiverThread(btSocket?.inputStream!!, serialLogger!!)
                            btReceiverThread?.start()

                            sendString("API\r")

                        } catch (e: IOException) {
                            connectionState?.text = getString(R.string.bt_timeout)
                            btSocket = null

                        }
                    }
                }
            }
        }else
        {
            btSocket?.inputStream?.close()
            btSocket?.outputStream?.close()
            btSocket?.close()
        }

    }

    fun sendString(str: String)
    {
        if(btSocket == null)
        {
            initConnection()
        }
        else
        {
            btSocket?.outputStream?.write(str.toByteArray())
        }

    }


}


class RGBSeekBarChangeListener(private var csFragment: ColorSelect, private var clr: Int) : SeekBar.OnSeekBarChangeListener
{


    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        if (clr== COLOR_RED) {
            csFragment.mainClr.r = progress
        } else if (clr ==COLOR_GREEN)
        {
            csFragment.mainClr.g = progress
        } else if (clr == COLOR_BLUE)
        {
            csFragment.mainClr.b = progress
        }


        csFragment.lampBallSelectorUpper?.setColorForSelected(csFragment.mainClr)
        csFragment.lampBallSelectorLower?.setColorForSelected(csFragment.mainClr)

        val lampsUpper = csFragment.lampBallSelectorUpper?.getSelectedString()
        val lampsLower = csFragment.lampBallSelectorLower?.getSelectedString()

        var clrCmd = "RGB(${csFragment.mainClr.r},${csFragment.mainClr.g},${csFragment.mainClr.b}"
        if (lampsUpper != null) {
            if (lampsUpper.isNotEmpty())
            {
                clrCmd += ",$lampsUpper"
            }
        }
        if (lampsLower != null) {
            if (lampsLower.isNotEmpty())
            {
                clrCmd += ",$lampsLower"
            }
        }
        clrCmd += ")\r"
        (csFragment.activity as MainActivity).sendString(clrCmd)

    }

    override fun onStartTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }
}

class BluetoothReceiverThread(var inputStream: InputStream,var logger: TextView): Thread()
{
    private val handler: Handler = Handler()
    var isRunning = true
    @Override
    override fun run() {
        while(isRunning)
        {
            val bytesAvailable = inputStream.available()
            if(bytesAvailable > 0)
            {
                val b = ByteArray(bytesAvailable)
                inputStream.read(b)
                val addedText = b.decodeToString()
                handler.post {
                    logger.text = addedText
                }

            }
            sleep(100)
        }
    }
}