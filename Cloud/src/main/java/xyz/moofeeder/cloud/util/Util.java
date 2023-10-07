package xyz.moofeeder.cloud.util;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.Reader;
import java.util.Base64;
import java.util.logging.Level;
import java.util.logging.Logger;
import java.util.stream.Stream;

public class Util
{
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
}

