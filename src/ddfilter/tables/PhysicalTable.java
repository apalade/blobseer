
package tables;

import data.MonitorData;
import data.PhysicalData;
import java.util.HashSet;
import util.TimeUtil;

public class PhysicalTable extends TableInterface {

  public PhysicalTable(DatabaseAccess dba) {
    super("physical", dba);
  }

  public void insert(MonitorData d) {
    PhysicalData data = (PhysicalData) d;

    String sqlcmd = "INSERT INTO " + escapeName(table + "_" + data.node)
            + " (rectime, " + buildColumns(data) + ")"
            + "VALUES (" + 
            + TimeUtil.getTimeSeconds(data.timestamp) + ", "
            + buildValues(data) 
            + ")";

    db.executeQuery(sqlcmd);
  }

  public void createTable(String node, HashSet<String> columns) {
    String tablename = table + "_" + node;
    String indexname = "idx_" + tablename;
    String sqlcmd = "CREATE TABLE " + escapeName(tablename)
            + "("
            + "rectime BIGINT, "
            + buildColumnsWithType(columns)
            + ")";
    db.executeQuery(sqlcmd);

    sqlcmd = "DROP INDEX IF EXISTS " + escapeName(indexname);
    db.executeQuery(sqlcmd);
    sqlcmd = "CREATE INDEX " + escapeName(indexname) + " ON " +
            escapeName(tablename) + "(rectime)";
    db.executeQuery(sqlcmd);
  }

  private String buildColumnsWithType(HashSet<String> cols) {
    String columns = "";

    for (String param_name: cols) {
      columns += escapeName(param_name) + " REAL, ";
    }

    return columns.substring(0, columns.length() - 2);

  }

  private String buildColumns(PhysicalData data) {
    String columns = "";

    for (String param_name: data.params.keySet()) {
      columns += escapeName(param_name) + ", ";
    }

    return columns.substring(0, columns.length() - 2);
  }
  
  private String buildValues(PhysicalData data) {
    String values = "";

    for (Object param_value: data.params.values()) {
      values += param_value.toString() + ", ";
    }

    return values.substring(0, values.length() - 2);
  }


}
