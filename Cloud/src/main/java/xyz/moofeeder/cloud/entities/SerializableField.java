package xyz.moofeeder.cloud.entities;

import xyz.moofeeder.cloud.enums.SerializableFieldType;

import java.lang.annotation.ElementType;
import java.lang.annotation.RetentionPolicy;
import java.lang.annotation.Retention;
import java.lang.annotation.Target;

@Target(ElementType.FIELD)
@Retention(RetentionPolicy.RUNTIME)
public @interface SerializableField
{
    String name();
    SerializableFieldType type();

    boolean encode() default false;

}
