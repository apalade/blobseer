package util;

import java.text.ParseException;
import java.text.SimpleDateFormat;
import java.util.Calendar;

public class TimeUtil {

  public static String now() {
    String DATE_FORMAT_NOW = "yyyy-MMM-dd HH:mm:ss.SSSSSS";
    Calendar cal = Calendar.getInstance();
    SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
    return sdf.format(cal.getTime());
  }

  public static long getTimeSeconds(String time) {
    String DATE_FORMAT_NOW = "yyyy-MMM-dd HH:mm:ss.SSSSSS";
    SimpleDateFormat sdf = new SimpleDateFormat(DATE_FORMAT_NOW);
    java.util.Date d = new java.util.Date();
    try {
      d = sdf.parse(time);
    } catch (ParseException e) {
      e.printStackTrace();
    }

    return d.getTime() / 1000;
  }
}
