package ch.sr35.balllampapp.backend

import android.os.Parcel
import android.os.Parcelable

const val FRAMERATE = 30.0

class Animation(var lampAnimations: ArrayList<LampAnimation>): Parcelable {

    var mappingLower: IntArray? = null
    var mappingUpper: IntArray? = null

    constructor(): this(ArrayList<LampAnimation>(1))

    constructor(parcel: Parcel) : this() {
        mappingLower = parcel.createIntArray()
        mappingUpper = parcel.createIntArray()
        parcel.createTypedArrayList(LampAnimation)
    }



    fun toCommandList(): Array<String>
    {
        val cmdList: ArrayList<String> = ArrayList()
        var currentColor = SimpleIntColor(0,0,0)
        var summedDuration: Long = 0
        var lastDuration: Long = 0
        var currentInterpolationType: InterpolationType = InterpolationType.CONSTANT
        //var cnt = 0
        var sameClrCnt = 0


        for (la in lampAnimations)
        {
            val lampidx: Int = if (la.lampNr < 10) {
                mappingUpper?.get(la.lampNr.toInt())!!
            } else {
                mappingLower?.get(la.lampNr.toInt()-10)!!
            }
            var cnt = 0
            val preliminaryStepList = ArrayList<String>()
            for (el in la.steps.withIndex())
            {
                if (el.index == 0)
                {
                    currentColor = el.value.color!!
                    summedDuration = el.value.duration
                    lastDuration = el.value.duration
                    currentInterpolationType = el.value.interpolation
                    sameClrCnt = 0
                }

                else {
                    if (el.value.color != currentColor) {

                        val interpolationType =
                            if (currentInterpolationType == InterpolationType.LINEAR) 1 else 0
                        if(sameClrCnt > 0)
                        {
                            preliminaryStepList.add("ISTEP(${currentColor.r},${currentColor.g},${currentColor.b},${summedDuration-lastDuration},0,${lampidx},${cnt})\r")
                            cnt++
                        }
                        preliminaryStepList.add("ISTEP(${currentColor.r},${currentColor.g},${currentColor.b},$lastDuration,$interpolationType,${lampidx},${cnt})\r")
                        cnt++
                        currentColor = el.value.color!!
                        summedDuration = el.value.duration
                        lastDuration = el.value.duration
                        currentInterpolationType = el.value.interpolation
                        sameClrCnt = 0
                    } else {
                        summedDuration += el.value.duration
                        lastDuration = el.value.duration
                        sameClrCnt++
                    }
                }
            }
            if (cnt >= preliminaryStepList.size)
            {
                val interpolationType =
                    if (currentInterpolationType == InterpolationType.LINEAR) 1 else 0

                if(sameClrCnt > 0)
                {
                    preliminaryStepList.add("ISTEP(${currentColor.r},${currentColor.g},${currentColor.b},${summedDuration-lastDuration},0,${lampidx},${cnt})\r")
                    cnt++
                }
                preliminaryStepList.add("ISTEP(${currentColor.r},${currentColor.g},${currentColor.b},$lastDuration,$interpolationType,${lampidx},${cnt})\r")
                cnt++
            }
            val repeatingInt = if (la.repeating && cnt > 1)  1 else 0

            cmdList.add("INTERP(${lampidx},$cnt,$repeatingInt)\r")
            cmdList.addAll(preliminaryStepList)
        }
        return cmdList.toTypedArray()
    }

    fun getByteSize(): Int
    {
        return lampAnimations.stream().mapToInt { e -> e.getByteSize() }.sum() + 33
    }

    fun getTotalDurationInSeconds(): Double
    {
        return lampAnimations.stream().mapToDouble{ e-> e.getTotalDurationInSeconds()}.max().orElse(0.0)
    }

    fun addTemporalOffset(seconds: Double)
    {
        for (la in lampAnimations) {
            if (la.steps[0].color == SimpleIntColor(0,0,0) && la.steps[0].interpolation == InterpolationType.CONSTANT)
            {
                la.steps[0].duration = (seconds*30).toLong()
            }
            else
            {
                la.steps.add(0,
                    Step(SimpleIntColor(0,0,0), (seconds*30).toLong(), InterpolationType.CONSTANT)
                )
            }
        }
    }

    override fun writeToParcel(parcel: Parcel, flags: Int) {
        parcel.writeIntArray(mappingLower)
        parcel.writeIntArray(mappingUpper)
        parcel.writeTypedArray(lampAnimations.toTypedArray(),0)
    }

    override fun describeContents(): Int {
        return 0
    }

    companion object CREATOR : Parcelable.Creator<Animation> {
        override fun createFromParcel(parcel: Parcel): Animation {
            return Animation(parcel)
        }

        override fun newArray(size: Int): Array<Animation?> {
            return arrayOfNulls(size)
        }
    }

}

class LampAnimation(var lampNr: Byte, var steps: ArrayList<Step>, var repeating: Boolean): Parcelable
{
    fun getByteSize(): Int
    {
        return if (steps.isNotEmpty()) {
            steps.distinct().stream().mapToInt { e -> e.getByteSize() }.sum() + 24
        }
        else
        {
            24
        }
    }

    fun getTotalDurationInSeconds(): Double
    {
        return steps.stream().mapToLong { e->e.duration }.sum()/ FRAMERATE
    }

    companion object CREATOR :Parcelable.Creator<LampAnimation> {
        override fun createFromParcel(source: Parcel?): LampAnimation {
            val lampNr = source?.readByte()
            val steps = source?.createTypedArrayList(Step)
            val repeating = source?.readByte()
            return if (lampNr != null && steps != null && repeating != null) {
                if (repeating > 0) {
                    LampAnimation(lampNr,steps,true)
                }else {
                    LampAnimation(lampNr,steps,false)
                }
            } else {
                LampAnimation(0,ArrayList(1),false)
            }
        }

        override fun newArray(size: Int): Array<LampAnimation> {
            return Array(size) { LampAnimation(0, ArrayList(1),false) }
        }

    }

    override fun describeContents(): Int {
        TODO("Not yet implemented")
    }

    override fun writeToParcel(dest: Parcel?, flags: Int) {
        dest?.writeByte(lampNr)
        dest?.writeTypedArray(steps.toTypedArray(),0)
        if (repeating) {
            dest?.writeByte(1)
        } else
        {
            dest?.writeByte(0)
        }
    }
}

class Step(var color: SimpleIntColor?, var duration: Long, var interpolation: InterpolationType): Parcelable {

    constructor(parcel: Parcel) : this(
        parcel.readParcelable(SimpleIntColor::class.java.classLoader),
        parcel.readLong(),
        InterpolationType.LINEAR
    ) {
        val interpolationType = parcel.readString()
        if (interpolationType != null)
        {
            interpolation = InterpolationType.valueOf(interpolationType)
        }
    }

    override fun equals(other: Any?): Boolean {
        return if (other is Step) {
            other.color == color
        } else {
            false
        }
    }

    fun getByteSize(): Int
    {
        return 20
    }

    override fun hashCode(): Int {
        return color.hashCode()
    }

    override fun writeToParcel(parcel: Parcel, flags: Int) {
        parcel.writeParcelable(color, flags)
        parcel.writeLong(duration)
    }

    override fun describeContents(): Int {
        return 0
    }

    companion object CREATOR : Parcelable.Creator<Step> {
        override fun createFromParcel(parcel: Parcel): Step {
            return Step(parcel)
        }

        override fun newArray(size: Int): Array<Step?> {
            return arrayOfNulls(size)
        }
    }
}



enum class InterpolationType{
    CONSTANT, LINEAR
}