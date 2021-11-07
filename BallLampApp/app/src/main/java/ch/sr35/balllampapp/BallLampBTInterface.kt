package ch.sr35.balllampapp
import android.bluetooth.*
import android.content.BroadcastReceiver
import android.content.Context
import android.content.Intent
import android.content.IntentFilter
import java.util.*


const val DEVICE_NAME = "BallLamp"
const val PIN = "3006"
val appUuid = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB")
class BallLampBTInterface(var btMan: BluetoothManager) {

    var btSocket: BluetoothSocket? = null

}

