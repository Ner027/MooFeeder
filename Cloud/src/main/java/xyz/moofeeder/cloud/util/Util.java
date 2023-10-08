package xyz.moofeeder.cloud.util;

import io.javalin.http.HttpStatus;
import org.bouncycastle.util.encoders.Base64Encoder;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.security.SecureRandom;
import java.util.Base64;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Stream;

public class Util
{
    private static final String m_validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    private static final int m_validCharsLen = m_validChars.length();
    private static final SecureRandom m_rng = new SecureRandom();

    /**
     * Returns a string built from lines read from an input reader
     * @param _reader Reader to try and read the string from
     * @return String containing content read from _reader
     */
    public static String stringFromReader(Reader _reader)
    {
        BufferedReader br = new BufferedReader(_reader);
        Stream<String> lines = br.lines();
        StringBuilder strBuilder = new StringBuilder();
        lines.forEach(strBuilder::append);

        try
        {
            br.close();
        }
        catch (IOException e)
        {
            Logger.getLogger(Util.class.getName()).log(Level.WARNING, "Failed to close a buffered reader!", e);
        }

        return strBuilder.toString();
    }

    public static int generateRandomInt(int lowerBound, int upperBound)
    {
        return m_rng.nextInt(lowerBound, upperBound);
    }
    public static String generateRandomString(int len)
    {
        StringBuilder strBuilder = new StringBuilder();

        for (int i = 0; i < len; i++)
            strBuilder.append(m_validChars.charAt(generateRandomInt(0, m_validCharsLen)));

        return strBuilder.toString();
    }

    public static void validateString(String src, HttpStatus status, RequestErrorCause cause) throws RequestException
    {
        if ((src == null) || src.isEmpty())
            throw new RequestException(status, cause);
    }
    public static String b64Encode(String src)
    {
        return new String(Base64.getEncoder().encode(src.getBytes()));
    }

    public static String b64Decode(String src)
    {
        return new String(Base64.getDecoder().decode(src));
    }
    public static void criticalExit()
    {
        System.exit(-1);
    }
}


