package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.data.DataManager;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.enums.RegisterForbiddenCause;
import xyz.moofeeder.cloud.rest.exceptions.RegisterForbidden;
import xyz.moofeeder.cloud.rest.handlers.IHandler;

import static xyz.moofeeder.cloud.enums.RegisterForbiddenCause.*;

public class RegisterHandler implements IHandler
{
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

        controlBox.setUsername(username);
        controlBox.setPassword(password);
        controlBox.insert();

        controlBox = new ControlBox();

        controlBox.load("username", username);
        System.out.println(controlBox.getUsername());
        System.out.println(controlBox.getPassword());
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
