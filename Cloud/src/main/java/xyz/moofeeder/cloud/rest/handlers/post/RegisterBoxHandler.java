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

import static xyz.moofeeder.cloud.util.Consts.PASSWORD_FIELD;
import static xyz.moofeeder.cloud.util.Consts.USERNAME_FIELD;

public class RegisterBoxHandler implements IHandler
{
    private final Logger m_logger = Logger.getLogger(getClass().getName());
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String username = ctx.formParam(USERNAME_FIELD);
        String password = ctx.formParam(PASSWORD_FIELD);

        Util.validateString(username, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_USER);
        Util.validateString(password, HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PASSWORD);

        Util.log("Starting register of new Control Box with user: " + username + " and password: " + password);

        ControlBox controlBox = new ControlBox();

        controlBox.load(USERNAME_FIELD, username);

        if (controlBox.getId() > 0)
        {
            Util.log("User already exists!");
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.USER_EXISTS);
        }

        if (!controlBox.setUsername(username))
        {
            Util.log("Invalid username!");
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_USER);
        }

        if (!controlBox.setPassword(password))
        {
            Util.log("Password invalid!");
            throw new RequestException(HttpStatus.FORBIDDEN, RequestErrorCause.INVALID_PASSWORD);
        }

        controlBox.refreshSessionToken();

        try
        {
            controlBox.insert();

            JSONObject jObj = new JSONObject();
            jObj.put("sessionToken", controlBox.getSessionToken());
            ctx.json(jObj.toString());
            ctx.status(HttpStatus.CREATED);
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
