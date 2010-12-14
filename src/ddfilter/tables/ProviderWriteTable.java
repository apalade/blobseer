package tables;

import util.TimeUtil;
import data.MonitorData;
import data.ProviderWriteData;

public class ProviderWriteTable extends TableInterface {

  public ProviderWriteTable(DatabaseAccess dba) {
    super("internal_write", dba);
  }

  public void insert(MonitorData d) {
    ProviderWriteData data = (ProviderWriteData) d;

    String sqlcmd = "INSERT INTO " + escapeName(table + "_" + data.node)
            + " (blob_id,rectime,watermark,page_index, page_size)"
            + "VALUES ("
            + data.blob_id + ","
            + TimeUtil.getTimeSeconds(data.timestamp) + ","
            + format(data.watermark) + ","
            + data.page_index + ","
            + data.page_size
            + ")";

    db.executeQuery(sqlcmd);
  }

  public void createTable(String node) {
    String tablename = table + "_" + node;
    String indexname = "idx_" + tablename;
    String sqlcmd = "CREATE TABLE " + escapeName(tablename)
            + "("
            + "blob_id BIGINT,"
            + "rectime BIGINT,"
            + "watermark VARCHAR(255),"
            + "page_index BIGINT,"
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
