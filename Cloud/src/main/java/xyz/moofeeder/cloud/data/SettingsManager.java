package xyz.moofeeder.cloud.data;

import org.json.JSONException;
import org.json.JSONObject;
import xyz.moofeeder.cloud.util.Util;

import java.io.*;
import java.util.HashMap;
import java.util.logging.Level;
import java.util.logging.Logger;
public class SettingsManager
{
    private static SettingsManager m_instance;
    private final HashMap<String,String> m_data = new HashMap<>();
    private final Logger m_logger = Logger.getLogger(getClass().getName());

    public static SettingsManager getInstance()
    {
        if (m_instance == null)
            m_instance = new SettingsManager();

        return m_instance;
    }

    private SettingsManager()
    {
        File file = new File("/home/andre/config.json");

        //If the config file does not exist yet, try to create it and load the default values
        if (!file.exists())
        {
            try
            {
                if (!file.createNewFile())
                {
                    m_logger.log(Level.SEVERE,  "Failed to create a new config file!");
                    System.exit(-1);
                }

                FileWriter fileWriter = new FileWriter(file);

                InputStream inputStream = getClass().getResourceAsStream("/defaults/defaultConfig.json");

                if (inputStream == null)
                {
                    if (!file.delete())
                        m_logger.log(Level.WARNING, "Unable to delete temporary config file! Maybe try to delete it yourself!");
                    m_logger.log(Level.SEVERE,  "Couldn't find default config file!");
                    System.exit(-1);
                }

                InputStreamReader inputStreamReader = new InputStreamReader(inputStream);

                fileWriter.write(Util.stringFromReader(inputStreamReader));
                inputStreamReader.close();
                fileWriter.close();
            }
            catch (IOException e)
            {
                m_logger.log(Level.SEVERE, "Couldn't create a new settings file!", e);
                System.exit(-1);
            }
        }

        //If the system was able to open the settings file, get the JSON data and parse it accordingly
        try
        {
            String content = Util.stringFromReader(new FileReader(file));
            JSONObject jObject = new JSONObject(content);

            jObject.keys().forEachRemaining(s -> {
                try
                {
                    m_data.put(s,jObject.getString(s));
                }
                catch (JSONException e)
                {
                    m_logger.log(Level.WARNING, "Invalid JSON Format!", e);
                }
            });
        }
        catch (FileNotFoundException e)
        {
            m_logger.log(Level.SEVERE, "Unable to find the settings file!", e);
            System.exit(-1);
        }
        catch (JSONException e)
        {
            m_logger.log(Level.WARNING, "Invalid JSON Format!", e);
            System.exit(-1);
        }

    }

    public String getProperty(String _key)
    {
        String obj = m_data.get(_key);

        if (obj == null)
        {
            m_logger.log(Level.SEVERE, "Property " + _key + " not found! Please check your config file");
            System.exit(-1);
        }

        return obj;
    }
}
