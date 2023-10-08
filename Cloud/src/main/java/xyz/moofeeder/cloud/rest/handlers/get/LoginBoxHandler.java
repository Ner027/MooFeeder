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

public class LoginBoxHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String username = ctx.formParam("username");
        String password = ctx.formParam("password");

        ControlBox controlBox = new ControlBox();

        Util.validateString(username, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_USER);
        Util.validateString(password, HttpStatus.UNAUTHORIZED, RequestErrorCause.INVALID_PASSWORD);

        controlBox.load("username", username);

        if (controlBox.getId() < 0)
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.USER_NOT_FOUND);

        if (!controlBox.checkAccess(password))
            throw new RequestException(HttpStatus.UNAUTHORIZED, RequestErrorCause.WRONG_PASSWORD);

        System.out.println("User " + username + " logged in successfully");

        try
        {
            controlBox.refreshSessionToken();
        }
        catch (Exception e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }

        JSONObject jObj = new JSONObject();
        jObj.put("session_token", controlBox.getSessionToken());
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
