package xyz.moofeeder.cloud.security;

import kotlin.Pair;
import org.springframework.security.crypto.argon2.Argon2PasswordEncoder;
import xyz.moofeeder.cloud.util.Util;
import xyz.moofeeder.cloud.util.Consts;

import java.nio.charset.StandardCharsets;
import java.security.SecureRandom;
import java.util.logging.Level;
import java.util.logging.Logger;

public class Encryption
{
    private static final Argon2PasswordEncoder m_encoder = new Argon2PasswordEncoder(
            Consts.saltLen,
            Consts.hashLen,
            Consts.encryptionMaxCores,
            Consts.encryptionMaxMemory,
            Consts.encryptionMaxRuns);
    private static final SecureRandom m_rng = new SecureRandom();
    private static final String m_pepper = System.getenv("KEY_PEPPER");
    private static final Logger m_logger = Logger.getLogger(Encryption.class.getName());

    public static Pair<String, String> encodePassword(String src)
    {
        if (m_pepper == null)
        {
            m_logger.log(Level.SEVERE, "System wide pepper not set!");
            Util.criticalExit();
        }

        //Generate random salt
        byte[] saltBytes = new byte[Consts.saltLen];
        m_rng.nextBytes(saltBytes);
        String saltString = new String(saltBytes, StandardCharsets.US_ASCII);

        return new Pair<>(saltString, m_encoder.encode(getComposedPassword(src, saltString)));
    }

    public static boolean checkPassword(String password, String salt, String encodedPassword)
    {
        return m_encoder.matches(getComposedPassword(password, salt), encodedPassword);
    }

    private static String getComposedPassword(String password, String salt)
    {
        return interlaceStrings(salt, m_pepper) + password;
    }
    private static String getComposedPassword(String password, byte[] salt)
    {
        return getComposedPassword(password, new String(salt, StandardCharsets.US_ASCII));
    }
    private static String interlaceStrings(String l, String r)
    {
        if ((l == null) || (r == null))
            return null;

        if (l.length() != r.length())
            return null;

        StringBuilder strBuilder = new StringBuilder();

        for (int i = 0; i < l.length(); i++)
            strBuilder.append(l.charAt(i)).append(r.charAt(i));

        return strBuilder.toString();
    }
}
