package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

public class LogoutBoxHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        long id = Util.validateToken(ctx.formParam("sessionToken"));

        if (!DataManager.insertData("LogoutControlBox", id))
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());

        ctx.status(HttpStatus.OK);
    }
    @Override
    public HandlerType getType()
    {
        return HandlerType.POST;
    }

    @Override
    public String getPath()
    {
        return "/box/logout";
    }
}
