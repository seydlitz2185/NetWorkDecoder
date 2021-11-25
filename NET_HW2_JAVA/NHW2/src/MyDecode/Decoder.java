package MyDecode;

public interface Decoder {
    void digitalLevelPairReader(int channel);
    void digitalJumpReader(int channel);
    long getTime(String s);
    int getTimeDifference(String a, String b) ;
    boolean isNewHighLevel();
    void resetTimeMap();
}
