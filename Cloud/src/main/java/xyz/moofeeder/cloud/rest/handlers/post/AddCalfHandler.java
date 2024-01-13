package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.entities.Calf;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Consts;
import xyz.moofeeder.cloud.util.Util;

import java.sql.SQLException;

public class AddCalfHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String sessionToken = ctx.formParam("sessionToken");
        String phyTag = ctx.formParam("phyId");
        String parentHwId = ctx.formParam("parentStation");

        long box_id = Util.validateToken(sessionToken);

        Util.validateString(phyTag, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);
        Util.validateString(parentHwId, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_HWID);

        if (phyTag.length() != Consts.PHY_ID_LEN)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);

        FeedingStation feedingStation = new FeedingStation();

        try
        {
            feedingStation.load("hw_id", parentHwId);
        }
        catch (SQLException | IllegalAccessException e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }

        long id = feedingStation.getId();

        if (id < 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_HWID);

        if (feedingStation.getParentId() != box_id)
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_HWID);

        Calf calf = new Calf();
        calf.load("phy_tag", phyTag);

        if (calf.getId() >= 0)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.CALF_EXISTS);

        calf.setParent(id);
        calf.setPhyTag(phyTag);

        try
        {
            calf.insert();
        }
        catch (SQLException | IllegalAccessException e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }

        ctx.status(HttpStatus.CREATED);
    }
    @Override
    public HandlerType getType()
    {
        return HandlerType.POST;
    }

    @Override
    public String getPath()
    {
        return "/calf/add";
    }
}
