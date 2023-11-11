package xyz.moofeeder.cloud.rest.handlers.patch;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import org.json.JSONObject;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

public class ChangeStationHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String sessionToken = ctx.formParam("sessionToken");
        String hwId = ctx.formParam("hwId");
        String name = ctx.formParam("name");
        String desc = ctx.formParam("description");

        Util.validateToken(sessionToken);
        Util.validateString(hwId, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_HWID);

        FeedingStation feedingStation = new FeedingStation();
        feedingStation.load("hw_id", hwId);

        if (feedingStation.getId() < 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.STATION_NOT_FOUND);

        int updateCount = 0;
        updateCount += (feedingStation.setName(name) ? 1 : 0);
        updateCount += (feedingStation.setDescription(desc) ? 1 : 0);

        try
        {
            if (updateCount > 0)
                feedingStation.update();

            ctx.status(HttpStatus.OK);
            JSONObject jObj = new JSONObject();
            jObj.put("updates", updateCount);
            ctx.json(jObj.toString());
            Util.log(   "Altered feeding station, set name to: " + name + " and description to " + desc);
        }
        catch (Exception e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }
    }

    @Override
    public HandlerType getType()
    {
        return HandlerType.PATCH;
    }

    @Override
    public String getPath()
    {
        return "/station/change";
    }
}
