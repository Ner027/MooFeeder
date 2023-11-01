package xyz.moofeeder.cloud.rest.exceptions;

import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import xyz.moofeeder.cloud.enums.RequestErrorCause;

import java.util.Map;

public class RequestException extends HttpResponseException
{
    public RequestException(HttpStatus status, RequestErrorCause cause)
    {
        super(status.getCode(), "RequestException", Map.of("cause", String.valueOf(cause.getValue())));
    }
}
