
package tables;

import data.MonitorData;
import data.ProviderReadData;
import util.TimeUtil;

public class ProviderReadTable extends TableInterface {

  public ProviderReadTable(DatabaseAccess dba) {
    super("internal_read", dba);
  }

  public void insert(MonitorData d) {
    ProviderReadData data = (ProviderReadData) d;

    String sqlcmd = "INSERT INTO " + escapeName(table + "_" + data.node)
            + " (page_id,blob_id,rectime,watermark, page_size)"
            + "VALUES ("
            + data.page_id + ","
            + data.blob_id + ","
            + TimeUtil.getTimeSeconds(data.timestamp) + ","
            + format(data.watermark) + ","
            + data.page_size
            + ")";

    db.executeQuery(sqlcmd);
  }

  public void createTable(String node) {
    String tablename = table + "_" + node;
    String indexname = "idx_" + tablename;
    String sqlcmd = "CREATE TABLE " + escapeName(tablename)
            + "("
            + "page_id BIGINT,"
            + "blob_id BIGINT,"
            + "rectime BIGINT,"
            + "watermark VARCHAR(255),"
            + "page_size BIGINT"
            + ")";
    db.executeQuery(sqlcmd);

    sqlcmd = "DROP INDEX IF EXISTS " + escapeName(indexname);
    db.executeQuery(sqlcmd);
    sqlcmd = "CREATE INDEX " + escapeName(indexname) + " ON " +
            escapeName(tablename) + "(rectime)";
    db.executeQuery(sqlcmd);
  }
}
