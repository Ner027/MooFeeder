package xyz.moofeeder.cloud.rest.handlers.delete;

import io.javalin.http.Context;
import io.javalin.http.HandlerType;
import io.javalin.http.HttpResponseException;
import io.javalin.http.HttpStatus;
import org.jetbrains.annotations.NotNull;
import xyz.moofeeder.cloud.entities.ControlBox;
import xyz.moofeeder.cloud.entities.FeedingStation;
import xyz.moofeeder.cloud.rest.handlers.IHandler;
import xyz.moofeeder.cloud.util.Util;

import java.sql.SQLException;
import java.util.LinkedList;

import static xyz.moofeeder.cloud.util.Util.tryLoginBox;

public class DeleteBoxHandler implements IHandler
{
    @Override
    public void handle(@NotNull Context ctx) throws Exception
    {
        ControlBox controlBox = tryLoginBox(ctx);

        LinkedList<FeedingStation> feedingStations = Util.getStationsByParentId(controlBox.getId());

        feedingStations.forEach(s -> {
            try
            {
                //TODO: Delete calves
                s.delete("id");
            }
            catch (SQLException | IllegalAccessException e)
            {
                throw new HttpResponseException(HttpStatus.INTERNAL_SERVER_ERROR.getCode());
            }
        });
    }

    @Override
    public HandlerType getType()
    {
        return HandlerType.DELETE;
    }
    @Override
    public String getPath()
    {
        return "/box/delete";
    }
}
