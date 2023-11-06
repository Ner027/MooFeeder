package xyz.moofeeder.cloud.util;

import io.javalin.http.Context;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.bouncycastle.util.encoders.Base64Encoder;
import org.json.JSONArray;
import org.json.JSONObject;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;

import java.awt.*;
import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.security.SecureRandom;
import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;
import java.util.Base64;
import java.util.LinkedList;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Stream;

public class Util
{
    private static final String m_validChars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";
    private static final int m_validCharsLen = m_validChars.length();
    private static final SecureRandom m_rng = new SecureRandom();
    private static final Logger m_debugLogger = Logger.getLogger("DEBUG");

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

    public static long validateToken(String sessionToken)
    {
        Util.validateString(sessionToken, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_TOKEN);

        Long id = DataManager.getData(Long.class, "ValidateSessionToken", "id", sessionToken);

        if (id == null)
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.TOKEN_EXPIRED);

        return id;
    }
    public static void log(String src)
    {
        System.out.println(src);
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

    public static ControlBox tryLoginBox(Context ctx) throws Exception
    {
        String username = ctx.formParam("username");
        String password = ctx.formParam("password");

        ControlBox controlBox = new ControlBox();

        Util.validateString(username, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_USER);
        Util.validateString(password, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_PASSWORD);

        controlBox.load("username", username);

        if (controlBox.getId() < 0)
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.USER_NOT_FOUND);

        if (!controlBox.checkAccess(password))
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.WRONG_PASSWORD);

        return controlBox;
    }

    public static LinkedList<FeedingStation> getStationsByParentId(long id) throws SQLException
    {
        LinkedList<FeedingStation> feedingStations = new LinkedList<>();

        PreparedStatement pStat = DataManager.prepareStatement("GetStationsByParent", id);

        ResultSet set = pStat.executeQuery();

        while (set.next())
        {
            try
            {
                FeedingStation feedingStation = new FeedingStation();
                feedingStation.loadFromSet(set);
                feedingStations.add(feedingStation);
            }
            catch (Exception e)
            {
                throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
            }
        }

        return feedingStations;
    }

}


