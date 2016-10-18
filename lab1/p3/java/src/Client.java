import java.io.DataInputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.OutputStreamWriter;
import java.net.Socket;
import java.util.Scanner;

public class Client {
    public static void main(String[] args) throws IOException {
        if (args.length < 2) {
            System.err.println("Missing reqiured parameters ip and port.");
            System.exit(-1);
        }

        String ip    = args[0];
        Integer port = Integer.parseInt(args[1]);

        Socket c = new Socket(ip, port);
        Scanner reader = new Scanner(System.in);

        String string;
        System.out.print("Please enter the string: ");
        string = reader.nextLine();

        DataInputStream  sockIn  = new DataInputStream(c.getInputStream());
        DataOutputStream sockOut = new DataOutputStream(c.getOutputStream());

        sockOut.writeShort(string.length());
        sockOut.write(string.getBytes());
        sockOut.flush();

        short newSize = sockIn.readShort();
        StringBuilder sb = new StringBuilder();
        for(int i = 0; i < newSize; i++) {
            sb.append((char) sockIn.readByte());
        }
        System.out.println("The reversed string is:" + sb.toString());
        reader.close();
        c.close();
    }
}