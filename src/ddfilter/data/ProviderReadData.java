package data;

import tables.ProviderReadTable;

public class ProviderReadData extends MonitorData {

  public String page_id;
  public String blob_id;
  public String watermark;
  public String farm;
  public String node;
  public String page_size;
  public String providerTimestamp;

  public ProviderReadData(String f, String n, String[] data) {
    // the monitored parameter:
    // page_id-----blob_id----watermark----page_size-------client------ provider timestamp

    super(n);
    farm = f;
    node = n;
    page_id = data[0];
    blob_id = data[1];
    watermark = data[2];
    page_size = data[3];
    providerTimestamp = data[5];

  }
}
