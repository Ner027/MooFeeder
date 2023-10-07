package xyz.moofeeder.cloud.rest.exceptions;

import io.javalin.http.HttpResponseException;
import xyz.moofeeder.cloud.enums.LoginUnauthorizedCause;

import java.util.Map;

public class LoginUnauthorized extends HttpResponseException
{
    public LoginUnauthorized(LoginUnauthorizedCause cause)
    {
        super(401, "LoginUnauthorized", Map.of("cause", String.valueOf(cause.getValue())));
    }
}
