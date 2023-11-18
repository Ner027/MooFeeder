package xyz.moofeeder.cloud.rest.handlers.get;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import org.json.JSONObject;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.enums.RequestErrorCause;
import xyz.moofeeder.cloud.rest.exceptions.RequestException;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

import static xyz.moofeeder.cloud.util.Util.tryLoginBox;

public class LoginBoxHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        ControlBox controlBox = tryLoginBox(ctx);

        try
        {
            controlBox.refreshSessionToken();
        }
        catch (Exception e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }

        JSONObject jObj = new JSONObject();
        jObj.put("sessionToken", controlBox.getSessionToken());
        ctx.json(jObj.toString());
        ctx.status(HttpStatus.ACCEPTED);
    }
    @Override
    public HandlerType getType()
    {
        return HandlerType.GET;
    }

    @Override
    public String getPath()
    {
        return "/box/login";
    }
}
