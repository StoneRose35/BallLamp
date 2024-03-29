package ch.sr35.balllampapp

import android.Manifest
import android.bluetooth.BluetoothAdapter
import android.bluetooth.BluetoothDevice
import android.bluetooth.BluetoothManager
import android.bluetooth.BluetoothSocket
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import android.content.pm.PackageManager
import android.os.Build
import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import android.os.Handler
import android.os.Looper
import android.view.Menu
import android.view.MenuItem
import android.widget.Button
import android.widget.SeekBar
import android.widget.TextView

import androidx.annotation.RequiresApi
import androidx.appcompat.widget.Toolbar
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import androidx.fragment.app.Fragment
import ch.sr35.balllampapp.backend.Animation
import ch.sr35.balllampapp.fragments.AnimationList
import ch.sr35.balllampapp.fragments.ColorSelect
import ch.sr35.balllampapp.fragments.SavedAnimations
import java.io.IOException
import java.io.InputStream


const val COLOR_RED = 0
const val COLOR_GREEN = 1
const val COLOR_BLUE = 2

class MainActivity : AppCompatActivity() {


    var btSocket: BluetoothSocket? = null
    var btDevice: BluetoothDevice? = null
    var btAdapter: BluetoothAdapter? = null
    var connectionInitActive: Boolean = true
    private val connectorThread: BluetoothConnectionThread = BluetoothConnectionThread(this)
    var btReceiverThread: BluetoothReceiverThread? = null


    private var csInstanceState: Fragment.SavedState? = null
    private var alInstanceState: Fragment.SavedState? = null
    private var saInstanceState: Fragment.SavedState? = null
    var csFragment: ColorSelect = ColorSelect()
    var alFragment: AnimationList = AnimationList()
    var saFragment: SavedAnimations = SavedAnimations()
    //val frameViewModelAnimation: FrameViewModelAnimation by viewModels()
    private var grants=0


    @RequiresApi(Build.VERSION_CODES.S)
    override fun onCreate(savedInstanceState: Bundle?) {
        val btMan: BluetoothManager = applicationContext.getSystemService(Context.BLUETOOTH_SERVICE) as BluetoothManager
        btAdapter = btMan.adapter
        connectionInitActive = true
        super.onCreate(savedInstanceState)

        setContentView(R.layout.activity_main)
        //frameViewModelAnimation.animationFrame.value?.duration=0.4

        if (ContextCompat.checkSelfPermission(applicationContext, Manifest.permission.BLUETOOTH_CONNECT)!=PackageManager.PERMISSION_GRANTED
            || ContextCompat.checkSelfPermission(applicationContext, Manifest.permission.BLUETOOTH_SCAN)!=PackageManager.PERMISSION_GRANTED
            || ContextCompat.checkSelfPermission(applicationContext, Manifest.permission.BLUETOOTH_ADMIN)!=PackageManager.PERMISSION_GRANTED
            || ContextCompat.checkSelfPermission(applicationContext, Manifest.permission.BLUETOOTH)!=PackageManager.PERMISSION_GRANTED
            )
        {
            requestPermissions(arrayOf(Manifest.permission.BLUETOOTH_CONNECT,Manifest.permission.BLUETOOTH_SCAN,Manifest.permission.BLUETOOTH_ADMIN,Manifest.permission.BLUETOOTH),42)
        }
        else
        {
            grants = 15
        }

        val restoredAnimation = savedInstanceState?.getParcelable<Animation>("anim")
        if (restoredAnimation != null)
        {
            alFragment.animation = restoredAnimation
        }


        setFragment(csFragment)
        val toolbar: Toolbar = findViewById(R.id.toolbar)
        setSupportActionBar(findViewById(R.id.toolbar))
        toolbar.showOverflowMenu()


        //frameViewModelAnimation.animationFrame.value?.editedStep=null

        if (grants== 15) {
            val btReceiver = BTReceiver(this)
            registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_FOUND))
            registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
            registerReceiver(btReceiver, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))


            if (btSocket?.isConnected == false || btSocket == null) {
                connectionInitActive = true
                initConnection()
            } else {
                csFragment.connectionState?.text = resources.getString(R.string.bt_connected)
                csFragment.view?.findViewById<Button>(R.id.btnConnect)?.text =
                    resources.getString(R.string.bt_btn_disconnect)
            }
        }
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        if (requestCode==42 && grantResults.sum()==0 )
        {
            grants = 15
            val btReceiver = BTReceiver(this)
            registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_FOUND))
            registerReceiver(btReceiver, IntentFilter(BluetoothDevice.ACTION_BOND_STATE_CHANGED))
            registerReceiver(btReceiver, IntentFilter(BluetoothAdapter.ACTION_STATE_CHANGED))


            if (btSocket?.isConnected == false || btSocket == null) {
                connectionInitActive = true
                initConnection()
            } else {
                csFragment.connectionState?.text = resources.getString(R.string.bt_connected)
                csFragment.view?.findViewById<Button>(R.id.btnConnect)?.text =
                    resources.getString(R.string.bt_btn_disconnect)
            }
        }
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        when(item.itemId)
        {
            R.id.nav_color_select -> {
                if (supportFragmentManager.fragments[0] !is ColorSelect) {
                    if (supportFragmentManager.fragments.contains(alFragment)) {
                        //frameViewModelAnimation.animationFrame.value?.editedStep = null
                        alInstanceState =
                            supportFragmentManager.saveFragmentInstanceState(alFragment)
                    } else if (supportFragmentManager.fragments.contains(saFragment)) {
                        saInstanceState =
                            supportFragmentManager.saveFragmentInstanceState(saFragment)
                    }
                    csFragment.setInitialSavedState(csInstanceState)
                    setFragment(csFragment)
                }
            }
            R.id.nav_animation_view -> {
                if (supportFragmentManager.fragments[0] !is AnimationList) {
                    if (supportFragmentManager.fragments.contains(csFragment)) {
                        csInstanceState =
                            supportFragmentManager.saveFragmentInstanceState(csFragment)
                    } else if (supportFragmentManager.fragments.contains(saFragment)) {
                        saInstanceState =
                            supportFragmentManager.saveFragmentInstanceState(saFragment)
                    }
                    alFragment.setInitialSavedState(alInstanceState)
                    setFragment(alFragment)
                }
            }
            R.id.nav_stored_animations -> {
                if (supportFragmentManager.fragments[0] !is SavedAnimations) {
                    if (supportFragmentManager.fragments.contains(alFragment)) {
                        //frameViewModelAnimation.animationFrame.value?.editedStep = null
                        alInstanceState =
                            supportFragmentManager.saveFragmentInstanceState(alFragment)
                    } else if (supportFragmentManager.fragments.contains(csFragment)) {
                        csInstanceState =
                            supportFragmentManager.saveFragmentInstanceState(csFragment)
                    }
                    saFragment.setInitialSavedState(saInstanceState)
                    setFragment(saFragment)
                }
            }
        }
        return true
    }


    override fun onPause() {
        super.onPause()
        btReceiverThread?.isRunning=false
    }

    override fun onResume() {
        super.onResume()
        if (btSocket?.isConnected == true) {
            btReceiverThread?.isRunning = true
            if (btReceiverThread?.isAlive != true) {
                btReceiverThread?.start()
            }
        }
    }

    override fun onCreateOptionsMenu(menu: Menu?): Boolean {
        menuInflater.inflate(R.menu.navigation_items,menu)
        return true
    }

    override fun onSaveInstanceState(outState: Bundle) {
        super.onSaveInstanceState(outState)
        outState.putParcelable("anim",alFragment.animation)
    }

    fun initConnection()
    {

            if (connectionInitActive) {
                if (btAdapter?.state == BluetoothAdapter.STATE_OFF) {
                    val btOnIntent = Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE)
                    connectionInitActive = false
                    if (ActivityCompat.checkSelfPermission(
                            this,
                            Manifest.permission.BLUETOOTH_CONNECT
                        ) != PackageManager.PERMISSION_GRANTED
                    ) {
                        return
                    }
                    startActivity(btOnIntent)
                } else {
                    btDevice =
                        btAdapter?.bondedDevices?.firstOrNull { e -> e.name == DEVICE_NAME }
                    if (btDevice == null) {

                        btAdapter?.startDiscovery()
                        connectionInitActive = false
                    } else {
                        connectionInitActive = false
                        try {
                            if (!connectorThread.isAlive)
                            {
                                connectorThread.start()
                            }
                        } catch (e: IOException)
                        {
                            csFragment.connectionState?.text = resources.getString(R.string.bt_timeout)
                        }
                        connectionInitActive=true
                    }
                }
            }
        }

    fun removeConnection()
    {
        btReceiverThread?.isRunning = false
        while (btReceiverThread?.isAlive == true)
        {
            Thread.sleep(10)
        }
        btSocket?.inputStream?.close()
        btSocket?.outputStream?.close()
        btSocket?.close()

        findViewById<Button>(R.id.btnConnect).text = resources.getString(R.string.bt_btn_connect)
        csFragment.connectionState?.text = resources.getString(R.string.bt_disconnected)
    }



    fun setFragment(fragment: Fragment)
    {

        supportFragmentManager.beginTransaction().apply {
            replace(R.id.main_frame,fragment)
            commit()
        }
    }

    fun sendString(str: String)
    {
        if(btSocket == null)
        {
            initConnection()
        }
        else if (btSocket?.isConnected != true)
        {
            return
        }
        else
        {
            try {
                btSocket?.outputStream?.write(str.toByteArray())
            }
            catch (e: IOException)
            {
                csFragment.connectionState?.text = resources.getString(R.string.bt_connection_error)
            }
        }
    }

}


class RGBSeekBarChangeListener(private var csFragment: ColorSelect, private var clr: Int) : SeekBar.OnSeekBarChangeListener
{
    override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
        if (fromUser) {
            when (clr) {
                COLOR_RED -> {
                    csFragment.mainClr.r = progress
                }
                COLOR_GREEN -> {
                    csFragment.mainClr.g = progress
                }
                COLOR_BLUE -> {
                    csFragment.mainClr.b = progress
                }

            }

            csFragment.lampBallSelectorUpper?.setColorForSelected(csFragment.mainClr)
            csFragment.lampBallSelectorLower?.setColorForSelected(csFragment.mainClr)

            csFragment.animationFrame?.lampdataUpper = csFragment.lampBallSelectorLower?.lampData
            csFragment.animationFrame?.lampdataUpper = csFragment.lampBallSelectorUpper?.lampData


            val lampsUpper = csFragment.lampBallSelectorUpper?.getSelectedString()
            val lampsLower = csFragment.lampBallSelectorLower?.getSelectedString()

            var clrCmd =
                "RGB(${csFragment.mainClr.r},${csFragment.mainClr.g},${csFragment.mainClr.b}"
            if (lampsUpper != null) {
                if (lampsUpper.isNotEmpty()) {
                    clrCmd += ",$lampsUpper"
                }
            }
            if (lampsLower != null) {
                if (lampsLower.isNotEmpty()) {
                    clrCmd += ",$lampsLower"
                }
            }
            clrCmd += ")\r"
            (csFragment.activity as MainActivity).sendString(clrCmd)
        }
    }

    override fun onStartTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }

    override fun onStopTrackingTouch(seekBar: SeekBar?) {
        //TODO("Not yet implemented")
    }
}

class BluetoothReceiverThread(private var inputStream: InputStream,private var logger: TextView): Thread()
{
    private val looper: Looper=Looper.getMainLooper()
    private val handler: Handler = Handler(looper)
    var isRunning = true
    var fullString: String = ""

    override fun run() {
        while(isRunning)
        {
            try {
                val bytesAvailable = inputStream.available()
                if (bytesAvailable > 0) {
                    val b = ByteArray(bytesAvailable)
                    inputStream.read(b)
                    fullString += b.decodeToString()
                    handler.post {
                        logger.text = fullString
                    }

                }
                sleep(100)
            } catch (e: IOException)
            {
                isRunning = false
            }
        }
    }
}


class BluetoothConnectionThread(private var caller: MainActivity): Thread() {

    private val looper: Looper=Looper.getMainLooper()
    private val handler: Handler = Handler(looper)

    @RequiresApi(Build.VERSION_CODES.S)
    override fun run()
    {

        try {
            if (ActivityCompat.checkSelfPermission(
                    caller,
                    Manifest.permission.BLUETOOTH_CONNECT
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                val permission = Manifest.permission.BLUETOOTH_CONNECT
                val permissions = Array(1){permission}
                ActivityCompat.requestPermissions(caller,  permissions,42)
            }
            caller.btSocket = caller.btDevice?.createInsecureRfcommSocketToServiceRecord(appUuid)
            caller.btSocket?.connect()

            while(caller.btSocket?.isConnected != true)
            {
                sleep(10)
            }
            handler.post {
                caller.csFragment.connectionState?.text = caller.resources.getString(R.string.bt_connected)
                caller.btReceiverThread = BluetoothReceiverThread(
                    caller.btSocket?.inputStream!!,
                    caller.csFragment.serialLogger!!
                )
                caller.btReceiverThread?.start()
                caller.sendString("API\r")
                caller.findViewById<Button>(R.id.btnConnect).text =
                    caller.resources.getString(R.string.bt_btn_disconnect)

            }
        }catch (e: IOException) {
            try {
                val createRfcommSocket =
                    caller.btSocket?.remoteDevice?.javaClass?.getDeclaredMethod(
                        "createRfcommSocket",
                        Int::class.java
                    )
                val fbSocket = createRfcommSocket?.invoke(
                    caller.btSocket?.remoteDevice,
                    1)
                caller.btSocket =  (fbSocket as? BluetoothSocket)
                caller.btSocket?.connect()
                while(caller.btSocket?.isConnected != true)
                {
                    sleep(10)
                }
                handler.post {
                    caller.csFragment.connectionState?.text =
                        caller.resources.getString(R.string.bt_connected)

                    caller.btReceiverThread = BluetoothReceiverThread(
                        caller.btSocket?.inputStream!!,
                        caller.csFragment.serialLogger!!
                    )
                    caller.btReceiverThread?.start()

                    caller.sendString("API\r")
                    caller.findViewById<Button>(R.id.btnConnect).text =
                        caller.resources.getString(R.string.bt_btn_disconnect)

                }
            } catch (e3: Exception) {
                caller.btSocket?.close()
                caller.btSocket = null
                handler.post {
                    caller.csFragment.connectionState?.text = caller.resources.getString(R.string.bt_timeout)
                }
            }
        } catch (e2: IllegalThreadStateException)
        {
            caller.btSocket = null
            handler.post {
                caller.csFragment.connectionState?.text =
                    caller.resources.getString(R.string.bt_timeout)
            }
        }
        caller.connectionInitActive=true
    }
}
