package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
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
            throw new RegisterForbidden(403, INVALID_USER.getValue());

        if (password == null)
            throw new RegisterForbidden(403, INVALID_PASSWORD.getValue());

        System.out.println("Starting register of new Control Box with user: " + username + " and password: " + password);

        ControlBox controlBox = new ControlBox();

        controlBox.load("username", username);

        if (controlBox.getId() != -1)
        {
            System.out.println("User already exists!");
            throw new RegisterForbidden(403, USER_EXISTS.getValue());
        }


        if (!controlBox.setUsername(username))
        {
            System.out.println("Username invalid!");
            throw new RegisterForbidden(403, INVALID_PASSWORD.getValue());
        }

        if (!controlBox.setPassword(password))
        {
            System.out.println("Password invalid!");
            throw new RegisterForbidden(403, INVALID_USER.getValue());
        }

        controlBox.insert();
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
