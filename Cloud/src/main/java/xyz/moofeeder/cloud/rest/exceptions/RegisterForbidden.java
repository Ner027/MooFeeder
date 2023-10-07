package xyz.moofeeder.cloud.rest.exceptions;

import io.javalin.http.HttpResponseException;
import org.jetbrains.annotations.NotNull;

import java.util.Map;

public class RegisterForbidden extends HttpResponseException
{
    public RegisterForbidden(int status, int cause)
    {
        super(status, "RegisterForbidden", Map.of("cause", String.valueOf(cause)));
    }
}
