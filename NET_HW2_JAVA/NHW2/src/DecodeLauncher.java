import MyDecode.Decode;

public class DecodeLauncher extends Decode {
    public static void main(String []args){
        System.out.println("MoNrRzMan:");
        //Decode.decode_morse(1);
        //signal_detect();
        //MyDecode.Decode.decode_nrz(4, 0);
        MyDecode.Decode.decode_rz(3);
        //MyDecode.Decode.decode_manch(2);
         //clkCheck(MORSE_PIN);
        //clkCheck(NRZER_PIN);
        //clkCheck(MANCH_PIN);

//        String jk = "010101000110100001100101001000000110011101110010011001010110000101110100001000000100001101101111011011010110110101110101011011100110100101110011011101000010000001010000011000010111001001110100011110010010000001101111011001100010000001000011011010000110100101101100110000100101110";
//        System.out.println(stringToAsc(jk));
    }
}
