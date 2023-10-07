package xyz.moofeeder.cloud.rest.exceptions;

import io.javalin.http.HttpResponseException;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.enums.CreateForbiddenCause;

import java.util.Map;

public class CreateForbidden extends HttpResponseException
{

    public CreateForbidden(CreateForbiddenCause cause)
    {
        super(403, "CreateError", Map.of("cause", String.valueOf(cause.getValue())));

    }
}
