package xyz.moofeeder.cloud.rest.handlers.delete;

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

public class DeleteStationHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String hwId = ctx.formParam("hwId");
        String sessionToken = ctx.formParam("sessionToken");

        Util.validateToken(sessionToken);
        Util.validateString(hwId, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_HWID);
        Util.validateString(sessionToken, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_TOKEN);

        FeedingStation feedingStation = new FeedingStation();
        feedingStation.load("hw_id", hwId);

        if (feedingStation.getId() < 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.STATION_NOT_FOUND);

        try
        {
            if (!feedingStation.delete("id"))
                throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());

            Util.log("Deleted station with HwId: " + hwId);

            ctx.status(HttpStatus.OK);
        }
        catch (Exception e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }
    }
    @Override
    public HandlerType getType()
    {
        return HandlerType.DELETE;
    }

    @Override
    public String getPath()
    {
        return "/station/delete";
    }
}
