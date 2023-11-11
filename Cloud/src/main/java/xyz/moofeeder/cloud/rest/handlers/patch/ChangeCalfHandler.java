package xyz.moofeeder.cloud.rest.handlers.patch;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpStatus;
import org.eclipse.jetty.websocket.api.Session;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.entities.Calf;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.entities.Log;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

public class ChangeCalfHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String token = ctx.formParam("sessionToken");
        String phyTag = ctx.formParam("phyId");
        String notes = ctx.formParam("notes");
        String consumption = ctx.formParam("consumption");

        long box_id = Util.validateToken(token);

        Calf calf = new Calf();

        calf.load("phy_tag", phyTag);

        if (calf.getId() < 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);

        FeedingStation feedingStation = new FeedingStation();
        feedingStation.load("station_id", calf.getParentId());

        if (feedingStation.getId() != box_id)
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.TOKEN_EXPIRED);

        if (consumption != null)
        {
            double consumptionValue;

            try
            {
                consumptionValue = Double.parseDouble(consumption);
            }
            catch (NumberFormatException e)
            {
                throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.VALUE_NAN);
            }

            Log log = new Log(feedingStation.getId(), consumptionValue);

            log.insert();
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
        return "/calf/change";
    }
}
