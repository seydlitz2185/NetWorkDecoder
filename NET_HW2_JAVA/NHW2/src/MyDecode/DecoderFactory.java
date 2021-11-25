package MyDecode;

import MyDecode.Decode;
import MyDecode.DecodeHelper;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Hashtable;

import static MyDecode.Decode.CIRCLE;
import static MyDecode.Decode.wait_jump;

public class DecoderFactory {
    public static Decoder getDecoder(int channel){
        switch (channel){
            case 1 : return new MorseDecoder();
            case 2 : return new ManchDecoder();
            case 3 : return new RzDecoder();
            case 4 : return new NrzDecoder();
            default: throw  new IllegalArgumentException();
        }
    }

    public static class MorseDecoder implements Decoder {

        HashMap<String,Long> time = new HashMap<>();
        {
            time.put("high",(long)0);
            time.put("low",(long)0);
            time.put("highFlag",(long)0);
            time.put("lowFlag",(long)0);

        }
        /*wait a pair of high and low level,return an array of time in unit of millisecond.*/

        @Override
        public void digitalLevelPairReader(int channel) {
            time.replace("highFlag",time.get("high"));
            time.replace("lowFlag",time.get("low"));
            if (Decode.wait_high(channel)){
                time.replace("high",DecodeHelper.get_ms());
            }
            if (Decode.wait_low(channel)){
                time.replace("low", DecodeHelper.get_ms());
            }
            //System.out.println(time.toString());
        }
        /*wait a pair of jumps,return an array of time in unit of millisecond.
         * time[0] :time of first jump
         * time[1] :time of second jump*/
        @Override
        public void digitalJumpReader(int channel) {}
        /*retrun last pair of level's time array
         * time[0] :time of last high level
         * time[1] :time of last low level*/
        @Override
        public long getTime(String s) {
            return time.get(s);
        }


        public int getTimeDifference(String a, String b) {
            return (int)(time.get(a)-time.get(b));
        }

        @Override
        public boolean isNewHighLevel() {
            return time.get("high")!=time.get("highFlag")||time.get("lowFlag")==0;
        }

        public long getLowFlag(){
            return getTime("lowFlag");
        }
        @Override
        public void resetTimeMap() {
            time.replace("high",(long)0);
            time.replace("low",(long)0);
            time.replace("highFlag",(long)0);
            time.replace("lowFlag",(long)0);

        }

        public int getHighLevelTimeDifference(){
            //System.out.println(getTimeDifference("low","high"));
            return getTimeDifference("low","high");
        }
        public int getLowLevelTimeDifference(){
            //System.out.println(getTimeDifference("high","lowFlag"));
            return getTimeDifference("high","lowFlag");
        }
    }

    public static class ManchDecoder implements Decoder {
         HashMap<String,Long> time = new HashMap<>();
        {
            time.put("low",(long)0);
            time.put("jump",(long)0);
            time.put("jumpFlag",(long)0);
        }
         HashMap<String,Integer> flag = new HashMap<>();
        {
            flag.put("bit",-1);
            flag.put("state",-1);
            flag.put("cnt",-1);
            flag.put("manchCharCounter",0);
            flag.put("manchWordCounter",0);
        }

        private HashMap<Integer,String> manchWordArray = new HashMap<>();
        private StringBuffer manchBuffer = new StringBuffer();
        private boolean frameStart = false;
        private boolean isGoodResult = true;
        private int badResultIndex = -1;
        private boolean thisBadResult = false;
        @Override
        public void digitalLevelPairReader(int channel) {}

        @Override
        public void digitalJumpReader(int channel) {
            time.replace("jumpFlag",time.get("jump"));
            if (wait_jump(channel,flag.get("state"))){
                time.replace("jump",DecodeHelper.get_ms());
                if(getJumpLevelTimeDifference() <= CIRCLE){
                    flag.replace("cnt",flag.get("cnt")+1);
                }
                if(flag.get("cnt") == 2 ||getJumpFlag() ==0){
                    flag.replace("cnt",0);
                    flag.replace("bit",Math.abs(flag.get("state")-1));
                    manchBuffer.append(flag.get("bit"));
                    flag.replace("manchCharCounter",flag.get("manchCharCounter")+1);
                }else if (((getJumpLevelTimeDifference() <=CIRCLE*2)&&(getJumpLevelTimeDifference() > CIRCLE))){
                    flag.replace("cnt",0);
                    flag.replace("bit",Math.abs(flag.get("bit")-1));
                    manchBuffer.append(flag.get("bit"));
                    flag.replace("manchCharCounter",flag.get("manchCharCounter")+1);
                }
            }else {
                time.replace("low",DecodeHelper.get_ms());
            }
            //System.out.println(manchBuffer);
        }

        public boolean isRobust(){
            checkGoodResult();
            return isGoodResult;
        }

        public void checkGoodResult(){
            if(!manchWordArray.containsValue("11001101")) {
                isGoodResult = true;
            }
        }

        public boolean isChar(){
            return getFlag("manchCharCounter")==8 ;
        }

        public boolean isNull(int s){
            if(!manchWordArray.containsKey(s)){
            return true;}else return false;
        }

        private boolean wasBadChar(int s){
            if(isNull(s)){return false;}
            return manchWordArray.get(s).equals("11001101");
        }

        public void manchCharAppender(){
            flag.replace("manchCharCounter",0);
            int index = getFlag("manchWordCounter");
            String buffer = manchBuffer.toString();
//            System.out.print(buffer);
//            System.out.print(',');
//            System.out.print(index);
//            System.out.print(',');
//            System.out.print(thisBadResult);
//            System.out.print(',');
//            System.out.print(badResultIndex);
//            System.out.println();
            System.out.print('.');
            /*simple check*/
            if(buffer.charAt(0)=='0'){
                manchWordArray.put(index,buffer);
            }else {
                reTry();
            }
        }

        public boolean getIsGoodResult(){
            return isGoodResult;
        }

        @Override
        public int getTimeDifference(String a, String b) {
            return (int)((long)time.get(a)-(long)time.get(b));
        }

        public int getJumpLevelTimeDifference(){
            return getTimeDifference("jump","jumpFlag");
        }

        public int getLowLevelTimeDifference(){
            //System.out.println(getTimeDifference("low","jump"));
            return getTimeDifference("low","jump");
        }

        @Override
        public boolean isNewHighLevel() {
            return false;
        }

        @Override
        public long getTime(String s) {
            return time.get(s);
        }

        public long getJumpFlag(){
            return getTime("jumpFlag");
        }

        public int getFlag(String s){
            return flag.get(s);
        }

        public boolean isFrameStart(){
            checkFrameStart();
            return frameStart;
        }

        public void getManchWordArray(){
            System.out.println();
           for (int i:manchWordArray.keySet()){
               System.out.print(Decode.stringToAscII(manchWordArray.get(i)));
           }
           System.out.println();
        }

        @Override
        public void resetTimeMap() {
            time.replace("low",(long)0);
            time.replace("jump",(long)0);
            time.replace("jumpFlag",(long)0);
        }

        public void resetFlagMap(){
            flag.replace("bit",-1);
            flag.replace("state",-1);
            flag.replace("cnt",-1);
            flag.replace("manchCharCounter",0);
            flag.replace("manchWordCounter",0);
        }

        public void resetState(){
            if(flag.get("cnt") == 0){
                flag.replace("state",flag.get("bit"));
            }else if (flag.get("cnt") == 1){
                flag.replace("state",Math.abs(flag.get("bit")-1));
            }else {
                flag.replace("state",Decode.LOW);
                flag.replace("cnt",0);
            }
        }

        private void resetCounter(){
            flag.replace("manchCharCounter",0);
        }

        public void initialization(){
            resetFlagMap();
            resetTimeMap();
            frameStart = false;
            manchBuffer = new StringBuffer();
        }

        public void reTry(){
            resetFlagMap();
            resetTimeMap();
            frameStart = false;
            manchBuffer = new StringBuffer();
            thisBadResult = false;
            System.out.println();
            System.out.println("Catastrophic failure,try decoding again:-(");
            Decode.wait_signal_start(2);
        }

        public void checkFrameStart(){
            if(!frameStart && manchBuffer.toString().equals("1111")){
                frameStart =true;
                manchBuffer = new StringBuffer();
                resetCounter();
            }
        }

    }

    public static class NrzDecoder extends MorseDecoder{
        public int getQuitTimeDifferent(){

            return getTimeDifference("low","lowFlag");
        }
    }

    public static class RzDecoder extends NrzDecoder{
        @Override
        public void digitalLevelPairReader(int channel) {
            time.replace("highFlag",time.get("high"));
            if (Decode.wait_high(channel)){
                time.replace("high",DecodeHelper.get_ms());
                time.replace("lowFlag",time.get("low"));
            }
            if (Decode.wait_low(channel)){
                time.replace("low", DecodeHelper.get_ms());
            }
            //System.out.println(time.toString());
        }
    }
}
