package MyDecode;

import com.sun.jna.Native;
import com.sun.jna.win32.StdCallLibrary;

public class DecodeHelper {
    
        public interface Dll extends StdCallLibrary {
        Dll INSTANCE = (Dll) Native.load("gpiodll", Dll.class);
        public int GetAsyncKeyState(int k);
        Dll API = (Dll) Native.load("user32", Dll.class);
        public long get_us(); //得到一个微秒级计数值，可用于计算时间差，
        public long get_ms(); //得到一个毫秒级计数值，可用于计算时间差
        public void delay_us(int d); //延迟指定的微秒
        public void delay_ms(int d); //延迟指定的毫秒
        public int open_dev(); //检测并打开USB接口，1：成功，0：失败
        public void close_dev(); //关闭USB接口
        public int digitalRead(int channel); //检测指定的信号线电平，返回1：高电平，0：低电平
        public int digitalWrite(int channel, int bt); //设置指定的信号线电平，参数bt为1：高电平，0：低电平，返回1:成功，0：失败
    }
    static boolean esc_key(){
        return (Dll.API.GetAsyncKeyState(0x1B) & 0x8000) > 0;
    }
    static long get_us(){ //得到一个微秒级计数值，可用于计算时间差，
        return Dll.INSTANCE.get_us();
    }
    static long get_ms(){ //得到一个毫秒级计数值，可用于计算时间差
        return Dll.INSTANCE.get_ms();
    }
    
    static void delay_us(int d){ //延迟指定的微秒
        Dll.INSTANCE.delay_us(d);
    }
    static void delay_ms(int d){ //延迟指定的毫秒
        Dll.INSTANCE.delay_ms(d);
    }
    static int open_dev(){ //检测并打开USB接口，1：成功，0：失败
        return Dll.INSTANCE.open_dev();
    }
    static void close_dev(){ //关闭USB接口
        Dll.INSTANCE.close_dev();
    }
    static int digitalRead(int channel){ //检测指定的信号线电平，返回1：高电平，0：低电平，如果连续失败，尝试重新插拔USB接口
    //请使用MicroUSB数据线，而不是纯充电线（只有电源没有信号线）
        return Dll.INSTANCE.digitalRead(channel);
    }
    static int digitalWrite(int channel, int bt){ //设置指定的信号线电平，参数bt为1：高电平，0：低电平，返回1:成功，0：失败
        return Dll.INSTANCE.digitalWrite(channel, bt);
    }


}