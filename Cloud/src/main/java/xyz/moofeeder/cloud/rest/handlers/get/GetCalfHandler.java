package xyz.moofeeder.cloud.rest.handlers.get;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import org.json.JSONArray;
import org.json.JSONObject;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.entities.Calf;
import xyz.moofeeder.cloud.entities.Log;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

import java.sql.PreparedStatement;
import java.sql.ResultSet;
import java.sql.SQLException;

import static xyz.moofeeder.cloud.util.Util.getCalfLogsById;

public class GetCalfHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String token = ctx.formParam("sessionToken");
        String phyTag = ctx.formParam("phyId");

        Util.validateToken(token);
        Util.validateString(phyTag, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);

        Calf calf = new Calf();

        calf.load("phy_tag", phyTag);

        if (calf.getId() < 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);

        int currentTime = (int) (System.currentTimeMillis() / 1000);

        JSONArray logArray = getCalfLogsById(calf.getId(), currentTime);

        JSONObject jObj = new JSONObject();

        calf.recalculateConsumption((int) (System.currentTimeMillis() / 1000));
        jObj.put("calfData", calf.dumpToJson());
        jObj.put("logData", logArray);

        ctx.json(jObj.toString());
        ctx.status(HttpStatus.OK);
    }

    @Override
    public HandlerType getType()
    {
        return HandlerType.GET;
    }

    @Override
    public String getPath()
    {
        return "/calf/data";
    }
}
