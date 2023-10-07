package xyz.moofeeder.cloud.rest.exceptions;

import io.javalin.http.HttpResponseException;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.enums.RegisterForbiddenCause;

import java.util.Map;

public class RegisterForbidden extends HttpResponseException
{
    public RegisterForbidden(RegisterForbiddenCause cause)
    {
        super(403, "RegisterForbidden", Map.of("cause", String.valueOf(cause.getValue())));
    }
}
