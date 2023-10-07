package xyz.moofeeder.cloud.rest.exceptions;

import io.javalin.http.HttpResponseException;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.enums.AuthErrorCause;

import java.util.HashMap;
import java.util.Map;

public class AuthError extends HttpResponseException
{
    public AuthError(AuthErrorCause cause)
    {
        super(401, "AuthError", Map.of("cause", String.valueOf(cause.getValue())));
    }
}
