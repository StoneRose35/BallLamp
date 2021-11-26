package ch.sr35.balllampapp

import android.os.Parcel
import androidx.test.platform.app.InstrumentationRegistry
import androidx.test.ext.junit.runners.AndroidJUnit4
import ch.sr35.balllampapp.backend.*

import org.junit.Test
import org.junit.runner.RunWith

import org.junit.Assert.*
import kotlin.math.abs
import kotlin.random.Random

/**
 * Instrumented test, which will execute on an Android device.
 *
 * See [testing documentation](http://d.android.com/tools/testing).
 */
@RunWith(AndroidJUnit4::class)
class InstrumentedTests {
    @Test
    fun useAppContext() {
        // Context of the app under test.
        val appContext = InstrumentationRegistry.getInstrumentation().targetContext
        assertEquals("ch.sr35.balllampapp", appContext.packageName)
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
            steps.add(
                Step(
                    SimpleIntColor(
                        abs(rg.nextInt() % 256),
                        abs(rg.nextInt() % 256),
                        abs(rg.nextInt() % 256)
                    ),
                    abs(rg.nextLong()%10000),if (rg.nextBoolean()) InterpolationType.LINEAR else InterpolationType.CONSTANT)
            )
        }
        al.add(LampAnimation(0, steps,true))
        val anim = Animation(al)
        anim.mappingUpper = IntArray(10){ it }
        anim.mappingLower = IntArray(10){ it+10}

        var p = Parcel.obtain()
        anim.writeToParcel(p,0)
        p.setDataPosition(0)
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

    @Test
    fun stepParcelTest()
    {
        val step1 = Step(SimpleIntColor(2, 4, 6), 234,InterpolationType.CONSTANT)
        val p = Parcel.obtain()
        step1.writeToParcel(p,0)
        p.setDataPosition(0)
        val step2 = Step.createFromParcel(p)

        assert(step2.duration == step1.duration)
        assert(step1.color == step2.color)
        assert(step1.interpolation == step2.interpolation)
    }

    @Test
    fun colorParcelTest()
    {
        val clr1 = SimpleIntColor(2,3,4)
        val p = Parcel.obtain()
        clr1.writeToParcel(p,0)
        p.setDataPosition(0)
        val clr2 = SimpleIntColor.createFromParcel(p)
        assert(clr2.g == clr1.g)

    }
}