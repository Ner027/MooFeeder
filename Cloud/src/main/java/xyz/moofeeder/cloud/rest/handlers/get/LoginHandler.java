package xyz.moofeeder.cloud.rest.handlers.get;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import netscape.javascript.JSObject;
import org.jetbrains.annotations.NotNull;
import org.json.JSONObject;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.enums.LoginUnauthorizedCause;
import xyz.moofeeder.cloud.rest.exceptions.LoginUnauthorized;
import xyz.moofeeder.cloud.rest.handlers.IHandler;

public class LoginHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String username = ctx.formParam("username");
        String password = ctx.formParam("password");

        ControlBox controlBox = new ControlBox();

        if (username == null)
            throw new LoginUnauthorized(LoginUnauthorizedCause.INVALID_USER);

        if (password == null)
            throw new LoginUnauthorized(LoginUnauthorizedCause.INVALID_PASSWORD);

        controlBox.load("username", username);

        if (controlBox.getId() < 0)
            throw new LoginUnauthorized(LoginUnauthorizedCause.USER_NOT_FOUND);

        if (!controlBox.checkAccess(password))
            throw new LoginUnauthorized(LoginUnauthorizedCause.WRONG_PASSWORD);

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
