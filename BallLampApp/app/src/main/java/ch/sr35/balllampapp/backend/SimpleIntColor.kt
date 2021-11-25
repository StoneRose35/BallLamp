package ch.sr35.balllampapp.backend

import android.os.Parcel
import android.os.Parcelable
import kotlin.random.Random

class SimpleIntColor(var r: Int, var g: Int, var b: Int): Parcelable {


    constructor(parcel: Parcel) : this(
        parcel.readInt(),
        parcel.readInt(),
        parcel.readInt()
    ) {
    }

    fun clone(): SimpleIntColor
    {
        return SimpleIntColor(this.r,this.g,this.b)
    }

    override fun equals(other: Any?): Boolean {
        if (other is SimpleIntColor)
        {
            return other.r == r && other.g == g && other.b == b
        }
        return false
    }

    override fun hashCode(): Int {
        return r*256*256 +g*256 + b
    }

    override fun toString(): String {
        return ("R: $r, G: $g, B: $b")
    }


    override fun writeToParcel(parcel: Parcel, flags: Int) {
        parcel.writeInt(r)
        parcel.writeInt(g)
        parcel.writeInt(b)
    }

    override fun describeContents(): Int {
        return 0
    }

    companion object CREATOR : Parcelable.Creator<SimpleIntColor> {
        override fun createFromParcel(parcel: Parcel): SimpleIntColor {
            return SimpleIntColor(parcel)
        }

        override fun newArray(size: Int): Array<SimpleIntColor?> {
            return arrayOfNulls(size)
        }

        var rg = Random(32)
        fun randomColor(): SimpleIntColor
        {

            return SimpleIntColor(rg.nextInt(0,255), rg.nextInt(0,255), rg.nextInt(0,255))
        }
    }
}