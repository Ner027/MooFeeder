package xyz.moofeeder.cloud.rest;

import io.javalin.Javalin;
import jakarta.annotation.Nonnull;
import xyz.moofeeder.cloud.data.SettingsManager;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.rest.handlers.get.LoginBoxHandler;
import xyz.moofeeder.cloud.rest.handlers.patch.ChangeStationHandler;
import xyz.moofeeder.cloud.rest.handlers.post.CreateStationHandler;
import xyz.moofeeder.cloud.rest.handlers.post.DeleteStationHandler;
import xyz.moofeeder.cloud.rest.handlers.post.RegisterBoxHandler;
import xyz.moofeeder.cloud.util.Util;

import java.util.logging.Level;
import java.util.logging.Logger;

public class RestServer
{
    private static RestServer m_instance;
    private final Logger m_logger = Logger.getLogger(getClass().getName());
    private Javalin m_server;
    private boolean m_initComplete;
    private RestServer()
    {
        int serverPort;

        m_initComplete = false;

        try
        {
            serverPort = Integer.parseInt(SettingsManager.getInstance().getProperty("rest_port"));
            m_server = Javalin.create().start(serverPort);
        }
        catch (RuntimeException e)
        {
            m_logger.log(Level.SEVERE, "Unable to acquire Rest Port from config file!", e);
            Util.criticalExit();
        }
    }
    public static RestServer getInstance()
    {
        if (m_instance == null)
            m_instance = new RestServer();

        return m_instance;
    }

    private void addHandler(@Nonnull IHandler handler)
    {
        m_server.addHandler(handler.getType(), handler.getPath(), handler);
    }

    public void initHandlers()
    {
        if (m_initComplete)
            return;

        addHandler(new RegisterBoxHandler());
        addHandler(new LoginBoxHandler());
        addHandler(new CreateStationHandler());
        addHandler(new DeleteStationHandler());
        addHandler(new ChangeStationHandler());
    }
}
