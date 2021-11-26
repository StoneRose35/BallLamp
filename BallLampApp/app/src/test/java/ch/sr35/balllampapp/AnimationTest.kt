package ch.sr35.balllampapp

import android.os.Parcel
import ch.sr35.balllampapp.backend.*
import org.junit.Test
import kotlin.math.abs
import kotlin.random.Random

class AnimationTest {

    @Test
    fun animationSetupTest()
    {
        val  rg = Random(42)
        val  al = ArrayList<LampAnimation>()
        for (i in 0..19)
        {
            val stepLength = rg.nextInt() % 50
            val steps = ArrayList<Step>()
            for (c in 0..stepLength)
            {
                steps.add(Step(SimpleIntColor(abs(rg.nextInt() % 256),abs(rg.nextInt() % 256),abs(rg.nextInt() % 256)),rg.nextLong(),if (rg.nextBoolean()) InterpolationType.LINEAR else InterpolationType.CONSTANT))
            }
            al.add(LampAnimation(i.toByte(), steps,true))
        }
        val anim = Animation(al)
        assert(anim.getByteSize() > 24)
    }

    @Test
    fun toCommandListTest()
    {
        val  rg = Random(42)
        val  al = ArrayList<LampAnimation>()

        val stepLength = rg.nextInt() % 50
        val steps = ArrayList<Step>()
        for (c in 0..stepLength)
        {
            steps.add(Step(SimpleIntColor(abs(rg.nextInt() % 256),abs(rg.nextInt() % 256),abs(rg.nextInt() % 256)),abs(rg.nextLong()%10000),if (rg.nextBoolean()) InterpolationType.LINEAR else InterpolationType.CONSTANT))
        }
        al.add(LampAnimation(0, steps,true))
        val anim = Animation(al)
        anim.mappingUpper = IntArray(10){ it }
        anim.mappingLower = IntArray(10){ it+10}
        val cmdList = anim.toCommandList()
        assert(cmdList[0].startsWith("INTERP"))
        assert(cmdList[1].substringAfter("(").substringBefore(")").split(",")[0].toInt() == anim.lampAnimations[0].steps[0].color?.r)
        assert(cmdList[1].substringAfter("(").substringBefore(")").split(",")[3].toLong() == anim.lampAnimations[0].steps[0].duration)
    }

    @Test
    fun toParcelTest()
    {

        val  rg = Random(42)
        val  al = ArrayList<LampAnimation>()

        val stepLength = rg.nextInt() % 50
        val steps = ArrayList<Step>()
        for (c in 0..stepLength)
        {
            steps.add(Step(SimpleIntColor(abs(rg.nextInt() % 256),abs(rg.nextInt() % 256),abs(rg.nextInt() % 256)),abs(rg.nextLong()%10000),if (rg.nextBoolean()) InterpolationType.LINEAR else InterpolationType.CONSTANT))
        }
        al.add(LampAnimation(0, steps,true))
        val anim = Animation(al)
        anim.mappingUpper = IntArray(10){ it }
        anim.mappingLower = IntArray(10){ it+10}

        var p = Parcel.obtain()
        anim.writeToParcel(p,0)
        val anim2 = Animation.createFromParcel(p)

        assert(anim.lampAnimations.size == anim2.lampAnimations.size)

        for (e1 in anim.mappingLower!!.withIndex())
        {
            assert(e1.value == anim2.mappingLower?.get(e1.index) ?: -1)
        }

        for (e1 in anim.mappingUpper!!.withIndex())
        {
            assert(e1.value == anim2.mappingUpper?.get(e1.index) ?: -1)
        }

        for (e1 in anim.lampAnimations.withIndex())
        {
            assert(e1.value.lampNr == anim2.lampAnimations[e1.index].lampNr)
            assert(e1.value.repeating == anim2.lampAnimations[e1.index].repeating)
            for (s1 in e1.value.steps.withIndex())
            {
                assert(s1.value.color == anim2.lampAnimations[e1.index].steps[s1.index].color)
                assert(s1.value.duration == anim2.lampAnimations[e1.index].steps[s1.index].duration)
                assert(s1.value.interpolation == anim2.lampAnimations[e1.index].steps[s1.index].interpolation)
            }
        }

    }

}