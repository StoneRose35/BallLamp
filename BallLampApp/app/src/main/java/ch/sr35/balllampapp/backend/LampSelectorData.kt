package ch.sr35.balllampapp.backend

import android.os.Parcel
import android.os.Parcelable

class LampSelectorData(var colors: Array<SimpleIntColor>, var selected: Array<Boolean>): Parcelable {

    /*init {
        selected = Array<Boolean>(10){ false }
        colors = Array<SimpleIntColor>(10){ SimpleIntColor(255,255,255) }
    }*/
    constructor(parcel: Parcel) : this(
        Array<SimpleIntColor>(10){ SimpleIntColor(255,255,255)},Array<Boolean>(10) {false}
    ) {
    }

    override fun writeToParcel(parcel: Parcel, flags: Int) {

    }

    override fun describeContents(): Int {
        return 0
    }

    companion object CREATOR : Parcelable.Creator<LampSelectorData> {
        override fun createFromParcel(parcel: Parcel): LampSelectorData {
            return LampSelectorData(parcel)
        }

        override fun newArray(size: Int): Array<LampSelectorData?> {
            return arrayOfNulls(size)
        }
    }
}