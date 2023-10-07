package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.apache.commons.logging.Log;
import org.jetbrains.annotations.NotNull;
import org.slf4j.event.Level;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.rest.exceptions.RegisterForbidden;
import xyz.moofeeder.cloud.rest.handlers.IHandler;

import java.util.logging.Logger;

import static xyz.moofeeder.cloud.enums.RegisterForbiddenCause.*;

public class RegisterHandler implements IHandler
{
    private final Logger m_logger = Logger.getLogger(getClass().getName());
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String username = ctx.formParam("username");
        String password = ctx.formParam("password");

        if (username == null)
            throw new RegisterForbidden(INVALID_USER);

        if (password == null)
            throw new RegisterForbidden(INVALID_PASSWORD);

        System.out.println("Starting register of new Control Box with user: " + username + " and password: " + password);

        ControlBox controlBox = new ControlBox();

        controlBox.load("username", username);

        if (controlBox.getId() > 0)
        {
            System.out.println("User already exists!");
            throw new RegisterForbidden(USER_EXISTS);
        }

        if (!controlBox.setUsername(username))
        {
            System.out.println("Username invalid!");
            throw new RegisterForbidden(INVALID_PASSWORD);
        }

        if (!controlBox.setPassword(password))
        {
            System.out.println("Password invalid!");
            throw new RegisterForbidden(INVALID_USER);
        }

        try
        {
            controlBox.insert();
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
