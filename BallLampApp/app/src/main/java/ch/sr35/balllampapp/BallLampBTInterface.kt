package ch.sr35.balllampapp
import android.Manifest
import android.bluetooth.*
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.pm.PackageManager
import androidx.core.app.ActivityCompat
import java.io.IOException
import java.util.*


const val DEVICE_NAME = "BallLamp"
val appUuid: UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")


class BTReceiver(private var mainActivity: MainActivity ): BroadcastReceiver()
{
    override fun onReceive(context: Context?, intent: Intent?) {
        if (intent?.action == BluetoothAdapter.ACTION_STATE_CHANGED)
        {
            val state = intent.getIntExtra(BluetoothAdapter.EXTRA_STATE,0)
            if (state == BluetoothAdapter.STATE_ON) {
                mainActivity.csFragment.connectionState?.text = mainActivity.resources.getString(R.string.bt_radio_on)
                mainActivity.connectionInitActive = true
                mainActivity.initConnection()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_FOUND)
        {
            val discoveredDevice = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
            if (ActivityCompat.checkSelfPermission(
                    mainActivity,
                    Manifest.permission.BLUETOOTH_CONNECT
                ) != PackageManager.PERMISSION_GRANTED
            ) {
                // TODO: Consider calling
                //    ActivityCompat#requestPermissions
                // here to request the missing permissions, and then overriding
                //   public void onRequestPermissionsResult(int requestCode, String[] permissions,
                //                                          int[] grantResults)
                // to handle the case where the user grants the permission. See the documentation
                // for ActivityCompat#requestPermissions for more details.
                mainActivity.requestPermissions(Array<String>(1){"Hallo"},1);
            }
            if( discoveredDevice?.name== DEVICE_NAME)
            {
                mainActivity.csFragment.connectionState?.text = mainActivity.resources.getString(R.string.bt_discovered_device, DEVICE_NAME)
                mainActivity.btAdapter?.cancelDiscovery()
                discoveredDevice.createBond()
            }
        }
        else if (intent?.action == BluetoothDevice.ACTION_BOND_STATE_CHANGED)
        {
            val bondState = intent.getIntExtra(BluetoothDevice.EXTRA_BOND_STATE,-1)
            if(bondState==BluetoothDevice.BOND_BONDED)
            {
                val bondedDevice = intent.getParcelableExtra<BluetoothDevice>(BluetoothDevice.EXTRA_DEVICE)
                mainActivity.btSocket = bondedDevice?.createRfcommSocketToServiceRecord(appUuid)
                try {
                    val connectorThread = BluetoothConnectionThread(mainActivity)
                    connectorThread.start()
                    mainActivity.csFragment.connectionState?.text = mainActivity.resources.getString(R.string.bt_connecting)

                } catch (e: IOException)
                {
                    mainActivity.btSocket = null
                    mainActivity.csFragment.connectionState?.text = mainActivity.resources.getString(R.string.bt_timeout)
                }
            }
        }
    }
}
