package xyz.moofeeder.cloud.rest.handlers.post;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.enums.AuthErrorCause;
import xyz.moofeeder.cloud.enums.CreateForbiddenCause;
import xyz.moofeeder.cloud.rest.exceptions.AuthError;
import xyz.moofeeder.cloud.rest.exceptions.CreateForbidden;
import xyz.moofeeder.cloud.rest.handlers.IHandler;

public class CreateHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        String name = ctx.formParam("name");
        String hwId = ctx.formParam("hwId");
        String sessionToken = ctx.formParam("sessionToken");

        if (name == null)
            throw new CreateForbidden(CreateForbiddenCause.INVALID_NAME);

        if (hwId == null)
            throw new CreateForbidden(CreateForbiddenCause.INVALID_HWID);

        if (sessionToken == null)
            throw new AuthError(AuthErrorCause.INVALID_TOKEN);

        System.out.println(sessionToken);
    }
    @Override

    public HandlerType getType()
    {
        return HandlerType.POST;
    }

    @Override
    public String getPath()
    {
        return "/station/create";
    }
}
