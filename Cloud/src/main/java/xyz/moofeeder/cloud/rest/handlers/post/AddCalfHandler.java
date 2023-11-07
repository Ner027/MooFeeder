package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Consts;
import xyz.moofeeder.cloud.util.Util;

public class AddCalfHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String sessionToken = ctx.formParam("sessionToken");
        String phyId = ctx.formParam("phyId");

        Util.validateToken(sessionToken);
        Util.validateString(phyId, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);

        if (phyId.length() != Consts.phyIdLen)
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PHY_ID);



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
