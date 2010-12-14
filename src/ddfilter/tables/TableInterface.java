
package tables;

import lia.web.utils.Formatare;
import data.MonitorData;

public abstract class TableInterface {

  protected String table;
  protected DatabaseAccess db;

  public TableInterface(String t, DatabaseAccess dba) {
    table = t;
    db = dba;
  }

  protected String format(final String i) {
    if (i == null) {
      return "null";
    }
    return "'" + Formatare.mySQLEscape(i) + "'";
  }

  public String escapeName(String name) {
    return "\"" + name + "\"";
  }

  public abstract void insert(MonitorData data);

  //public abstract void createTable(String clientId);
}

