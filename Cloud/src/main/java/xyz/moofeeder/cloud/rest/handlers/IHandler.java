package xyz.moofeeder.cloud.rest.handlers;

import io.javalin.http.Handler;
import io.javalin.http.HandlerType;

public interface IHandler extends Handler
{
    HandlerType getType();
    String getPath();
}
