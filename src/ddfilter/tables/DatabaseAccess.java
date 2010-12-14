package tables;

import java.io.FileInputStream;
import java.io.IOException;
import java.sql.*;
import java.util.Properties;
import java.util.logging.Logger;

public class DatabaseAccess {

  private Connection conn = null;
  private static Logger logger = Logger.getLogger(DatabaseAccess.class.getName());
  private ResultSet rs;
  private Properties properties;

  public DatabaseAccess(String fileName) {
    properties = new Properties();
    try {
      FileInputStream fis = new FileInputStream(fileName);
      properties.load(fis);
      fis.close();
    } catch (IOException e) {
      System.err.println("Could not open properties file " + fileName);
    }
    initializeDB();
  }

  public void executeQuery(String sqlQuery) {
    try {
      Statement stat = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
      stat.executeQuery(sqlQuery);
      rs = stat.getResultSet();
    } catch (SQLException e) {
      logger.info(e.toString());
    }

  }

  public boolean queryResult(String sqlQuery) {
    try {
      Statement stat = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
      boolean st = stat.execute(sqlQuery, Statement.NO_GENERATED_KEYS);
      rs = stat.getResultSet();
      if (st) {
        return true;
      }

    } catch (SQLException e) {
      logger.info(e.toString());
    }
    return false;

  }

  public ResultSet query(String sqlQuery) {
    try {
      Statement stat = conn.createStatement(ResultSet.TYPE_SCROLL_INSENSITIVE, ResultSet.CONCUR_READ_ONLY);
      stat.execute(sqlQuery, Statement.NO_GENERATED_KEYS);
      rs = stat.getResultSet();

    } catch (SQLException e) {
      logger.info(e.toString());
    }
    return rs;
  }

  public String getDbURL() {
    String driverString = properties.getProperty("lia.Monitor.jdbcDriverString");
    String dbURL = new String();

    if (driverString.indexOf("postgres") != -1) {
      dbURL = "jdbc:postgresql://" + properties.getProperty("lia.Monitor.ServerName", "127.0.0.1") + ":" + properties.getProperty("lia.Monitor.DatabasePort", "5432") + "/" + properties.getProperty("lia.Monitor.DatabaseName", "mon_data");
    } else if (driverString.indexOf("mysql") != -1) {
      dbURL = "jdbc:mysql://" + properties.getProperty("lia.Monitor.ServerName", "127.0.0.1") + ":" + properties.getProperty("lia.Monitor.DatabasePort", "5432") + "/" + properties.getProperty("lia.Monitor.DatabaseName", "mon_data");
    }

    return dbURL;
  }

  public void initializeDB() {
    try {
      String dbURL = getDbURL();
      conn = DriverManager.getConnection(dbURL, 
              properties.getProperty("lia.Monitor.UserName", "mon_user"),
              properties.getProperty("lia.Monitor.Pass", "mon_pass"));
      // use connection
    } catch (SQLException e) {
      System.err.println("Connecting to database error: " + e.toString());
    } finally {
      if (conn == null) {
        System.err.println("Could not connect to database!");
      }
    }
  }
}