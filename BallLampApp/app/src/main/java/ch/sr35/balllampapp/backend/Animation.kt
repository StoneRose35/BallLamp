package ch.sr35.balllampapp

import ch.sr35.balllampapp.backend.SimpleIntColor

const val FRAMERATE = 30.0

class Animation(var lampAnimations: ArrayList<LampAnimation>) {

    var mappingLower: IntArray? = null
    var mappingUpper: IntArray? = null
    fun toCommandList(): Array<String>
    {
        var cmdList: ArrayList<String> = ArrayList<String>()
        var currentColor = SimpleIntColor(0,0,0)
        var currentDuration: Long = 0
        var currentInterpolationType: InterpolationType = InterpolationType.CONSTANT
        var cnt = 0


        for (la in lampAnimations)
        {
            var lampidx: Int
            if (la.lampNr < 10)
            {
                lampidx = mappingUpper?.get(la.lampNr.toInt())!!
            }
            else
            {
                lampidx = mappingLower?.get(la.lampNr.toInt()-10)!!
            }
            cnt = 0
            var preliminaryStepList = ArrayList<String>()
            for (el in la.steps.withIndex())
            {
                if (el.index == 0)
                {
                    currentColor = el.value.color
                    currentDuration = el.value.duration
                    currentInterpolationType = el.value.interpolation
                }

                else {
                    if (el.value.color != currentColor) {

                        val interpolationType =
                            if (currentInterpolationType == InterpolationType.LINEAR) 1 else 0
                        preliminaryStepList.add("ISTEP(${currentColor.r},${currentColor.g},${currentColor.b},$currentDuration,$interpolationType,${lampidx},${cnt})\r")
                        currentColor = el.value.color
                        currentDuration = el.value.duration
                        currentInterpolationType = el.value.interpolation
                        cnt++
                    } else {
                        currentDuration += el.value.duration
                    }
                }
            }
            if (cnt >= preliminaryStepList.size)
            {
                val interpolationType =
                    if (currentInterpolationType == InterpolationType.LINEAR) 1 else 0
                preliminaryStepList.add("ISTEP(${currentColor.r},${currentColor.g},${currentColor.b},$currentDuration,$interpolationType,${lampidx},${cnt})\r")
                cnt++
            }
            var repeatingInt = if (la.repeating && cnt > 1)  1 else 0

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
                la.steps.add(0,Step(SimpleIntColor(0,0,0), (seconds*30).toLong(),InterpolationType.CONSTANT))
            }
        }
    }
}

class LampAnimation(var lampNr: Byte,var steps: ArrayList<Step>,var repeating: Boolean)
{
    fun getByteSize(): Int
    {
        return steps.stream().distinct().mapToInt{ e->e.getByteSize()}.sum() + 24
    }

    fun getTotalDurationInSeconds(): Double
    {
        return steps.stream().mapToLong { e->e.duration }.sum()/ FRAMERATE
    }
}

class Step(var color: SimpleIntColor, var duration: Long, var interpolation: InterpolationType) {

    override fun equals(other: Any?): Boolean {
        if (other is Step)
        {
            return other.color == color
        }
        else
        {
            return false
        }
    }

    fun getByteSize(): Int
    {
        return 20
    }
}



enum class InterpolationType{
    CONSTANT, LINEAR
}