package xyz.moofeeder.cloud;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.rest.RestServer;


public class Main
{
    public static void main(String[] args)
    {
        DataManager.initDatabases();
        RestServer.getInstance().initHandlers();
    }
}
