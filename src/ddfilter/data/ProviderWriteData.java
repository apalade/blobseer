
package data;


public class ProviderWriteData extends MonitorData {

  public String blob_id;
  public String page_index;
  public String watermark;
  public String farm;
  public String node;
  public String page_size;
  public String providerTimestamp;

  public ProviderWriteData(String f, String n, String[] data) {
    super(n);

    farm = f;
    node = n;

    // the monitored parameter:
    // blob_id----index---watermark----page_size-------client------provider timestamp
    blob_id = data[0];
    page_index = data[1];
    watermark = data[2];
    page_size = data[3];

    providerTimestamp = data[5];
  }

}