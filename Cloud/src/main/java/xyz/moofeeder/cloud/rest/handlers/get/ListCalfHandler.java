package xyz.moofeeder.cloud.rest.handlers.get;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONObject;
import xyz.moofeeder.cloud.entities.Calf;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

import java.util.LinkedList;

public class ListCalfHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String token = ctx.formParam("sessionToken");

        long id = Util.validateToken(token);

        LinkedList<FeedingStation> stations = Util.getStationsByParentId(id);

        LinkedList<Calf> calves = new LinkedList<>();

        stations.forEach(feedingStation ->
        {
            try
            {
                LinkedList<Calf> temp = Util.getCalvesByParentId(feedingStation.getId());
                calves.addAll(temp);
            }
            catch (Exception e)
            {
                throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
            }
        });


        JSONArray jArray = new JSONArray();
        int currentTime = (int) (System.currentTimeMillis() / 1000);

        calves.forEach(s ->
        {
            try
            {
                s.recalculateConsumption(currentTime);
                jArray.put(s.dumpToJson());
            }
            catch (IllegalAccessException e)
            {
                throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
            }
        });

        JSONObject jsonObject = new JSONObject();

        jsonObject.put("list", jArray);
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
        return "/calf/list";
    }
}
