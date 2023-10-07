package xyz.moofeeder.cloud;
import org.springframework.security.crypto.argon2.Argon2PasswordEncoder;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.rest.RestServer;

public class Main
{
    public static void main(String[] args)
    {
        String testStr = "Hello";
        DataManager.initDatabases();
        RestServer.getInstance().initHandlers();
        Argon2PasswordEncoder encoder = new Argon2PasswordEncoder(16, 32, 4, 100000, 8);
        CharSequence seq = encoder.encode(testStr);
        System.out.println(seq);
        System.out.println(encoder.matches("Hello", seq.toString()));
    }
}
