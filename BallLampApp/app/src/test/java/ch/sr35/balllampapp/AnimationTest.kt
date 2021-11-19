package ch.sr35.balllampapp

import ch.sr35.balllampapp.backend.SimpleIntColor
import org.junit.Test
import kotlin.math.abs
import kotlin.random.Random

class AnimationTest {

    @Test
    fun animationSetupTest()
    {
        val  rg = Random(42)
        var  al = ArrayList<LampAnimation>()
        for (i in 0..19)
        {
            var stepLength = rg.nextInt() % 50
            var steps = ArrayList<Step>()
            for (c in 0..stepLength)
            {
                steps.add(Step(SimpleIntColor(abs(rg.nextInt() % 256),abs(rg.nextInt() % 256),abs(rg.nextInt() % 256)),rg.nextLong(),if (rg.nextBoolean()) InterpolationType.LINEAR else InterpolationType.CONSTANT))
            }
            al.add(LampAnimation(i.toByte(), steps,true))
        }
        var anim = Animation(al)
        assert(anim.getByteSize() > 24)
    }

    @Test
    fun toCommandListTest()
    {
        val  rg = Random(42)
        var  al = ArrayList<LampAnimation>()

        var stepLength = rg.nextInt() % 50
        var steps = ArrayList<Step>()
        for (c in 0..stepLength)
        {
            steps.add(Step(SimpleIntColor(abs(rg.nextInt() % 256),abs(rg.nextInt() % 256),abs(rg.nextInt() % 256)),abs(rg.nextLong()%10000),if (rg.nextBoolean()) InterpolationType.LINEAR else InterpolationType.CONSTANT))
        }
        al.add(LampAnimation(0, steps,true))
        var anim = Animation(al)
        val cmdList = anim.toCommandList()
        assert(cmdList[0].startsWith("INTERP"))
        assert(cmdList[1].substringAfter("(").substringBefore(")").split(",")[0].toInt() == anim.lampAnimations[0].steps[0].color.r)
        assert(cmdList[1].substringAfter("(").substringBefore(")").split(",")[3].toLong() == anim.lampAnimations[0].steps[0].duration)
    }
}