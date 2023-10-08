package xyz.moofeeder.cloud.rest.handlers.get;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONObject;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;
import java.sql.PreparedStatement;
import java.sql.ResultSet;

public class ListStationHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String sessionToken = ctx.formParam("sessionToken");

        Util.validateString(sessionToken, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_TOKEN);
        long id = Util.validateToken(sessionToken);

        PreparedStatement pStat = DataManager.prepareStatement("GetStationsByParent", id);

        ResultSet set = pStat.executeQuery();

        FeedingStation feedingStation = new FeedingStation();
        JSONObject jsonObject = new JSONObject();
        JSONArray jsonArray = new JSONArray();

        while (set.next())
        {
            try
            {
                feedingStation.loadFromSet(set);
                jsonArray.put(feedingStation.dumpToJson());
            }
            catch (Exception e)
            {
                throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
            }
        }

        jsonObject.put("list", jsonArray);
        ctx.status(HttpStatus.OK);
        ctx.json(jsonObject.toString());
    }

    @Override
    public HandlerType getType()
    {
        return HandlerType.GET;
    }

    @Override
    public String getPath()
    {
        return "/station/list";
    }
}
