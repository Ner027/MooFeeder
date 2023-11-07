package xyz.moofeeder.cloud;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.rest.RestServer;

import java.util.logging.Level;
import java.util.logging.Logger;
public class Main
{
    private static final Logger m_logger = Logger.getLogger(Main.class.getName());
    public static void main(String[] args)
    {
        m_logger.log(Level.INFO, "Starting MooFeeder Cloud System!");
        DataManager.initDatabase();
        RestServer.getInstance().initHandlers();
    }
}
