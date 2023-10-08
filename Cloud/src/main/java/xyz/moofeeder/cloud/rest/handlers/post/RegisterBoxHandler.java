package xyz.moofeeder.cloud.rest.handlers.post;

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

import java.util.logging.Logger;

public class RegisterBoxHandler implements IHandler
{
    private final Logger m_logger = Logger.getLogger(getClass().getName());
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String username = ctx.formParam("username");
        String password = ctx.formParam("password");

        Util.validateString(username, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_USER);
        Util.validateString(password, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PASSWORD);

        System.out.println("Starting register of new Control Box with user: " + username + " and password: " + password);

        ControlBox controlBox = new ControlBox();

        controlBox.load("username", username);

        if (controlBox.getId() > 0)
        {
            System.out.println("User already exists!");
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.USER_EXISTS);
        }

        if (!controlBox.setUsername(username))
        {
            System.out.println("Username invalid!");
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_USER);
        }

        if (!controlBox.setPassword(password))
        {
            System.out.println("Password invalid!");
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PASSWORD);
        }

        controlBox.refreshSessionToken();

        try
        {
            controlBox.insert();

            ctx.status(HttpStatus.CREATED);
            JSONObject jObj = new JSONObject();
            jObj.put("session_token", controlBox.getSessionToken());
            ctx.json(jObj.toString());
            ctx.status(HttpStatus.ACCEPTED);
        }
        catch (Exception e)
        {
            throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
        }
    }

    @Override
    public HandlerType getType()
    {
        return HandlerType.POST;
    }

    @Override
    public String getPath()
    {
        return "/box/register";
    }
}
