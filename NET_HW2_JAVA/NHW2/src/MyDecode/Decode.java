package MyDecode;

import MyDecode.DecodeHelper;

import java.util.*;

public class Decode {
    static int REPEAT_TIME = 2;
    static int FRAME_INTERVAL = 2000;
    static int TIMEOUT = 1200;
    static int HIGH = 1;
    static int LOW = 0;
    static int UNKNOWN = -1;
    static int ERR = -1;

    static int MORSE_PIN = 1; // 莫尔斯电报码信号线
    static int MANCH_PIN = 2; // 曼彻斯特码信号线
    static int RZERO_PIN = 3; // 归零码信号线
    static int NRZER_PIN = 4; // 非归零码信号线
    static int NRCLK_PIN = 0; // 时钟信号线
    static int CIN_PIN = 5; //自定义输入信号线
    static int COUT_PIN0 = 6; //自定义输出信号线1
    static int COUT_PIN1 = 7; //自定义输出信号线2
    static int COUT_PIN2 = 8; //自定义输出信号线3
    static int CIRCLE = 75; //信号周期

    private static final Map<String, String> MorseDict;
    private static final Map<String,String> ASCIIDict;
    static {
	//莫尔斯电报码表，0为短，1为长
        MorseDict = new HashMap<String, String>();
        MorseDict.put("01", "a");
        MorseDict.put("1000", "b");
        MorseDict.put("1010", "c");
        MorseDict.put("100", "d");
        MorseDict.put("0", "e");
        MorseDict.put("0010", "f");
        MorseDict.put("110", "g");
        MorseDict.put("0000", "h");
        MorseDict.put("00", "i");
        MorseDict.put("0111", "j");
        MorseDict.put("101", "k");
        MorseDict.put("0100", "l");
        MorseDict.put("11", "m");
        MorseDict.put("10", "n");
        MorseDict.put("111", "o");
        MorseDict.put("0110", "p");
        MorseDict.put("1101", "q");
        MorseDict.put("010", "r");
        MorseDict.put("000", "s");
        MorseDict.put("1", "t");
        MorseDict.put("001", "u");
        MorseDict.put("0001", "v");
        MorseDict.put("011", "w");
        MorseDict.put("1001", "x");
        MorseDict.put("1011", "y");
        MorseDict.put("1100", "z");
    }
    static {
        ASCIIDict = new HashMap<>();
        ASCIIDict.put("00100000"," ");
        ASCIIDict.put("00110000","0");
        ASCIIDict.put("00110001","1");
        ASCIIDict.put("00110010","2");
        ASCIIDict.put("00110011","3");
        ASCIIDict.put("00110100","4");
        ASCIIDict.put("00110101","5");
        ASCIIDict.put("00110110","6");
        ASCIIDict.put("00110111","7");
        ASCIIDict.put("00111000","8");
        ASCIIDict.put("00111001","9");
        ASCIIDict.put("00111010",":");
        ASCIIDict.put("00111011",";");
        ASCIIDict.put("01000001","A");
    }

    //等待高电平出现或超时
    static boolean wait_high(int channel) {
        long cnt = DecodeHelper.get_ms();
        while (0 == DecodeHelper.digitalRead(channel)) {
            if (DecodeHelper.get_ms() - cnt > TIMEOUT) {
                return false;
            }
        }
        return true;
    }

    //等待低电平出现或超时
    static boolean wait_low(int channel) {
        long cnt = DecodeHelper.get_ms();
        while (1 == DecodeHelper.digitalRead(channel)) {
            if (DecodeHelper.get_ms() - cnt > TIMEOUT) {
                return false;
            }
        }
        return true;
    }

    //等待跳变出现或超时
    static boolean wait_jump(int channel, int state) {
        if (UNKNOWN == state) {
            state = DecodeHelper.digitalRead(channel);
        }
        long cnt = DecodeHelper.get_ms();
        while (DecodeHelper.digitalRead(channel) == state) {
            if (DecodeHelper.get_ms() - cnt > TIMEOUT) {
                return false;
            }
        }
        return true;
    }

    //等待上一段信号结束，发生TIMEOUT以上的低电平
    static void wait_signal_end(int channel) {
        long cnt = DecodeHelper.get_ms();
        while (true) {
            if (DecodeHelper.digitalRead(channel) == HIGH) {
                cnt = DecodeHelper.get_ms();// 遇到高电平重新计时
            }
            if (DecodeHelper.get_ms() - cnt > TIMEOUT) {
                break;
            }
        }
    }

    //等待信号开始(上一段信号结束后的第一个高电平出现)
    static void wait_signal_start(int channel) {
        System.out.print("Wait next frame...\r\n");
        wait_signal_end(channel);
        System.out.print("Wait frame head...\r\n");
        while (DecodeHelper.digitalRead(channel) == LOW);
    }

    //莫尔斯电报码
    public static void decode_morse(int channel) {
        while (DecodeHelper.open_dev()==0);

        HashMap<String,ArrayList> morseWords = new HashMap<>();
        ArrayList<ArrayList> morseWordArray = new ArrayList<>();
        String morseWordArrayToString ;
        ArrayList<StringBuffer> morseCharArray = new ArrayList<>();
        StringBuffer morseChar = new StringBuffer();
        ArrayList<ArrayList> goodResult = null;

        DecoderFactory.MorseDecoder decoder = (DecoderFactory.MorseDecoder) DecoderFactory.getDecoder(channel);

        System.out.println("Decoding morse codes~~~");
        wait_signal_start(channel);

        while (true){
            decoder.digitalLevelPairReader(channel);
            if(decoder.isNewHighLevel()){
                if(decoder.getLowLevelTimeDifference()<= CIRCLE ||decoder.getLowFlag()==0 ){
                    if(decoder.getLowFlag() ==0 ){
                        morseCodesAppender(75,morseChar);
                    }else {
                        morseCodesAppender(decoder.getHighLevelTimeDifference(), morseChar);
                    }
                }else if(decoder.getLowLevelTimeDifference()<= CIRCLE*3){
                    morseCharArray.add(morseChar);
                    morseChar = new StringBuffer();
                    morseCodesAppender(decoder.getHighLevelTimeDifference(),morseChar);
                }else if(decoder.getLowLevelTimeDifference()<= CIRCLE*7){
                    morseCharArray.add(morseChar);
                    morseWordArray.add(morseCharArray);
                    morseCharArray = new ArrayList<>();
                    morseChar = new StringBuffer();
                    morseCodesAppender(decoder.getHighLevelTimeDifference(),morseChar);
                }
            }else {
                morseCharArray.add(morseChar);
                morseWordArray.add(morseCharArray);
                morseWordArrayToString = morseWordArray.toString();
                //System.out.println(morseWordArray);
                if(morseWords.containsKey(morseWordArrayToString)){
                    System.out.println("Decode Succeed!");
                    goodResult = morseWordArray;
                    break;
                }else{
                    morseWords.put(morseWordArrayToString,morseWordArray);
                    morseWordArray = new ArrayList<ArrayList>();
                    morseCharArray = new ArrayList<StringBuffer>();
                    morseChar = new StringBuffer();
                    decoder.resetTimeMap();
                    System.out.println();
                    System.out.println("Decode again,robustness test~~~");
                    wait_signal_start(channel);

                }
            }
            if(morseWords.size()>REPEAT_TIME){
                System.out.println("Multiple decode results, please identify manually:-(");
                break;
            }
        }

        System.out.println();

        if(goodResult!= null){
            System.out.println("Decoded morse codes:");
            for (Object w:goodResult) {
                for (Object s: (ArrayList)w) {
                    System.out.print(MorseDict.get(s.toString()));
                }
                System.out.print(' ');
            }
            System.out.println();
        }else {
            for (String ws: morseWords.keySet()) {
                for (Object w:morseWords.get(ws)) {
                    for (Object s: (ArrayList)w) {
                        System.out.print(MorseDict.get(s.toString()));
                    }
                    System.out.print(' ');
                }
                System.out.println();
            }
        }
    }

    //曼彻斯特解码
    public static void decode_manch(int channel){
        while (DecodeHelper.open_dev()==0);

        System.out.println("Decoding manchester codes~~~");
        wait_signal_start(channel);
        DecoderFactory.ManchDecoder decoder = (DecoderFactory.ManchDecoder) DecoderFactory.getDecoder(channel);

        while (true){
            decoder.resetState();
            decoder.digitalJumpReader(channel);
            if(decoder.isFrameStart()){
                if (decoder.getLowLevelTimeDifference() < TIMEOUT) {
                    if (decoder.isChar()) {
                        decoder.manchCharAppender();
                    }
                } else if (decoder.getLowLevelTimeDifference() >= TIMEOUT) {
                    if (decoder.getLowLevelTimeDifference() > FRAME_INTERVAL) {
                        if (decoder.isRobust()) {
                            System.out.println();
                            System.out.println("Decoding Succeed:-)");

                            decoder.getManchWordArray();
                            break;
                        }
                        decoder.getManchWordArray();
                        decoder.reTry();

                    }
                }
            }
        }

    }

    //非归零码
    public static void decode_nrz(int channel, int clk_channel) {

        while (DecodeHelper.open_dev()==0);

        StringBuffer nrzFrame = new StringBuffer();
        HashSet<String> nrzString = new HashSet<>();
        StringBuffer nrzBuffer = new StringBuffer();
        String goodResult = null;

        boolean frameStart = false ;
        DecoderFactory.NrzDecoder decoder = (DecoderFactory.NrzDecoder) DecoderFactory.getDecoder(channel);

        System.out.println("Decoding nrz codes~~~");
        wait_signal_start(channel);

        while (true){
            decoder.digitalLevelPairReader(channel);
            if(decoder.getHighLevelTimeDifference()<=CIRCLE* 4 && !frameStart){
                frameStart =true;
                decoder.digitalLevelPairReader(channel);
            }

            if( frameStart &&decoder.getQuitTimeDifferent() <= CIRCLE*8){
                for(int i = 0; i<(decoder.getLowLevelTimeDifference())/(CIRCLE-1);i++) {
                    nrzBuffer.append(0);
                }
                for (int i = 0; i<(decoder.getHighLevelTimeDifference())/(CIRCLE-1);i++){
                    nrzBuffer.append(1);
                }
                if(decoder.isNewHighLevel()){
                    nrzFrame.append(nrzBuffer);
                    nrzBuffer = new StringBuffer();
                }
            }else if(decoder.getQuitTimeDifferent() >CIRCLE*8){
                frameStart = false;
                if(nrzString.contains(nrzFrame.toString())) {
                    System.out.println("Decode Succeed!:-)");
                    goodResult =  nrzFrame.toString();
                    break;
                }
                nrzString.add(nrzFrame.toString());
                nrzFrame = new StringBuffer();
                nrzBuffer = new StringBuffer();
                decoder.resetTimeMap();
                System.out.println();
                System.out.println("Decode again,robustness test~~~");
                wait_signal_start(channel);
                }
            if(nrzString.size()>REPEAT_TIME){
                System.out.println("Multiple decode results, please identify manually:-(");
                break;
            }

        }
        System.out.println();
        if(goodResult!=null){
            System.out.println("Decoded nrz codes:");
            System.out.println(stringToAscII(goodResult));
        }else {
            for (Object s :nrzString.toArray()) {
                System.out.println(stringToAscII((String) s));
            }
        }
    }

    //归零码
    public static void decode_rz(int channel) {

        while (DecodeHelper.open_dev()==0);
        StringBuffer rzFrame = new StringBuffer();
        HashSet<String> rzString = new HashSet<>();
        String goodResult = null;
        boolean frameStart = true ;

        DecoderFactory.RzDecoder decoder = (DecoderFactory.RzDecoder) DecoderFactory.getDecoder(channel);

        System.out.println("Decoding rz codes~~~");
        wait_signal_start(channel);

        while (true){
            decoder.digitalLevelPairReader(channel);
            if( (frameStart &&decoder.getQuitTimeDifferent() <=FRAME_INTERVAL )||decoder.getLowFlag()==0){
                if(decoder.isNewHighLevel()){
                   // System.out.print('.');
                    if(decoder.getHighLevelTimeDifference() <=CIRCLE){
                        rzFrame.append(0);
                    }else if(decoder.getHighLevelTimeDifference() <=CIRCLE*2){
                        rzFrame.append(1);
                    }
                }
            }else if(decoder.getQuitTimeDifferent() >=FRAME_INTERVAL&&decoder.getLowFlag()!=0){
                frameStart = false;
                if(rzString.contains(rzFrame.toString())) {
                    System.out.println("Decoding succeed:-)");
                    goodResult = rzFrame.toString();
                    break;
                }else {
                    rzString.add(rzFrame.toString());
                    rzFrame = new StringBuffer();
                    System.out.println();
                    System.out.println("Decode again,robustness test~~~");
                    decoder.resetTimeMap();
                    wait_signal_start(channel);
                    frameStart = true;
                }
            }
            if(rzString.size()>REPEAT_TIME ){
                System.out.println("Multiple decode results, please identify manually:-(");
                break;
            }
        }
        System.out.println();
        if(goodResult!=null){
            System.out.println("Decoded rz codes:");
            System.out.println(stringToAscII(goodResult));
        }else {
            System.out.println("All things are difficult before they are easy.");
            for (Object s :rzString.toArray()
            ) {
                System.out.println(stringToAscII((String) s));
            }
        }
    }

    //输出电平演示，实现三灯流水灯效果
    static void light_demo() {
        DecodeHelper.digitalWrite(COUT_PIN0, LOW);
        DecodeHelper.digitalWrite(COUT_PIN1, LOW);
        DecodeHelper.digitalWrite(COUT_PIN2, LOW);
        for (int i = 0; i < 200; i++) {
            DecodeHelper.digitalWrite(COUT_PIN0, i % 3 % 2 > 0 ? HIGH : LOW);
            DecodeHelper.digitalWrite(COUT_PIN1, (i + 1) % 3 % 2 > 0 ? HIGH : LOW);
            DecodeHelper.digitalWrite(COUT_PIN2, (i + 2) % 3 % 2 > 0 ? HIGH : LOW);
            DecodeHelper.delay_ms(50);
        }
        DecodeHelper.digitalWrite(COUT_PIN0, LOW);
        DecodeHelper.digitalWrite(COUT_PIN1, LOW);
        DecodeHelper.digitalWrite(COUT_PIN2, LOW);
    }

    //以5毫秒为周期检测指定的信号线电平变化并显示在屏幕，按ESC结束
    static void signal_detect() {
        while (true) {

            int channel = 0;
            System.out.println("\r\nSelect channel:");
            System.out.println("1: NRZ");
            System.out.println("2: Morse");
            System.out.println("3: Manchester");
            System.out.println("4: RZ");
            System.out.println("5: CLK");
            System.out.println("6: DATA_IN");
            System.out.println("0: Back");

            Scanner input = new Scanner(System.in);
            String chc = input.next();
            if ("0".equals(chc)) {
                break;
            }
            switch (chc) {
                case "1":
                    channel = NRZER_PIN;
                    break;
                case "2":
                    channel = MORSE_PIN;
                    break;
                case "3":
                    channel = MANCH_PIN;
                    break;
                case "4":
                    channel = RZERO_PIN;
                    break;
                case "5":
                    channel = NRCLK_PIN;
                    break;
                case "6":
                    channel = CIN_PIN;
                    break;
                default:
                    continue;
            }

            //等待信号出现
            while (DecodeHelper.digitalRead(channel) == HIGH);
            while (DecodeHelper.digitalRead(channel) == LOW);
            DecodeHelper.delay_ms(25);

            while (true) {
                if (DecodeHelper.esc_key()) //ESC键结束
                {
                    break;
                }
                System.out.print(DecodeHelper.digitalRead(channel) == HIGH ? "▉" : "▓");
                DecodeHelper.delay_ms(25);
            }
        }
    }

    /*check clock*/
    public static void clkCheck(int channel){
        while (DecodeHelper.open_dev()==0);
        DecodeHelper.open_dev();
        long high = 0,low=0,highFlag =0,lowFlag =0;
        wait_signal_start(channel);
        while (true) {
            if (DecodeHelper.esc_key())
            {
                DecodeHelper.close_dev();
                break;
            }

            if (wait_high(channel)){
                high = DecodeHelper.get_ms();
            }

            if (wait_low(channel)){
                low = DecodeHelper.get_ms();
            }
//            if (wait_jump(channel,LOW)){
//                high = MyDecode.DecodeHelper.get_ms();
//            }
//            if (wait_jump(channel,HIGH)){
//                low = MyDecode.DecodeHelper.get_ms();
//            }
            if(high-lowFlag >0||lowFlag ==0){
                System.out.print(high);
                System.out.print(",");
                System.out.print(low);
                System.out.print(",");
                System.out.print(low-high);
                System.out.print(",");
                System.out.print(high-lowFlag);
                System.out.println();
                //highFlag=high;
                lowFlag =low;
            }else {
                System.out.println(low-lowFlag);
            }
        }

    }

    /*Append morse codes into StringBuffer*/
    static void morseCodesAppender(int a, StringBuffer s){
        if(a<=CIRCLE){
            s.append(0);
        }else if(a<=CIRCLE*3){
            s.append(1);
        }
    }

    //MyDecode.Decode String to ASCII
    public static String stringToAscII(String s) {
        StringBuffer stringDecoder = new StringBuffer();
        StringBuffer cache = new StringBuffer();
        int intCache = 0;
        for (int i = 0; i < s.length()/8; i++) {
            for (int j = 0; j < 8; j++) {
                cache.append(s.charAt((i * 8) + j));
                //intCache += Integer.parseInt((String) s.charAt((i * 8) + j));
            }

            //System.out.println((char)Integer.parseInt(cache.toString(),2));
            stringDecoder.append((char)Integer.parseInt(cache.toString(),2));
            cache = new StringBuffer();
        }
        return stringDecoder.toString();
    }

    public static void main(String[] args) {

        if (0 == DecodeHelper.open_dev()) {
            System.out.println("Device not found or busy!");
            System.out.println("Please insert device to start");
            while (DecodeHelper.open_dev()==0);
        } else {
            System.out.println("Device opened.");
        }

        while (true) {
            System.out.println();
            System.out.println("1: Morse");
            System.out.println("2: Manchester");
            System.out.println("3: NRZ");
            System.out.println("4: RZ");
            System.out.println("5: Signal detect");
            System.out.println("6: Output DEMO");
            System.out.println("0: Quit");
            Scanner input = new Scanner(System.in);
            String chc = input.next();
            if ("0".equals(chc)) {
                break;
            }

            switch (chc) {
                case "1":
                    decode_morse(MORSE_PIN);
                    break;
                case "2":
                    decode_manch(MANCH_PIN);
                    break;
                case "3":
                    decode_nrz(NRZER_PIN, NRCLK_PIN);
                    break;
                case "4":
                    decode_rz(RZERO_PIN);
                    break;
                case "5":
                    signal_detect();
                    break;
                case "6":
                    light_demo();
                    break;
                default:
                    continue;
            }
        }

        DecodeHelper.close_dev();
    }

}
