package data;

import java.util.HashMap;

public class PhysicalData extends MonitorData {
  public String farm;
  public String node;
  public HashMap<String, Object> params;
  public Object[] values;

  public PhysicalData(String f, String n, HashMap<String, Object> params) {
    super(n);
    this.farm = f;
    this.node = n;
    this.params = params;
  }
}
