package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

import java.rmi.server.UID;
import java.sql.SQLException;

public class CreateStationHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String name = ctx.formParam("name");
        String hwId = ctx.formParam("hwId");
        String sessionToken = ctx.formParam("sessionToken");

        long id = Util.validateToken(sessionToken);
        Util.validateString(name, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_ST_NAME);
        Util.validateString(hwId, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_HWID);

        FeedingStation feedingStation = getFeedingStation(hwId, name, id);

        System.out.println("Creating a new feeding station with name: " + name + " and HwId: " + hwId + " for ControlBox with id: " + id);

        try
        {
            feedingStation.insert();
            /*TODO: Accordingly to the HTTP Standard this should return something, a reserved field is already defined
              in the specification*/
        }
        catch (Exception e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }
    }
    @Override

    public HandlerType getType()
    {
        return HandlerType.POST;
    }

    @Override
    public String getPath()
    {
        return "/station/create";
    }

    private static FeedingStation getFeedingStation(String hwId, String name, long id) throws SQLException, IllegalAccessException
    {
        FeedingStation feedingStation = new FeedingStation();
        feedingStation.load("hw_id", hwId);

        if (feedingStation.getId() > 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.STATION_EXISTS);

        feedingStation = new FeedingStation();

        if (!feedingStation.setName(name))
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_ST_NAME);

        if (!feedingStation.setHwId(hwId))
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_HWID);

        if (!feedingStation.setParentId(id))
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.TOKEN_EXPIRED);
        return feedingStation;
    }
}
