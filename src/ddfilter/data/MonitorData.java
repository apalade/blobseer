package data;

import java.util.HashSet;
import util.TimeUtil;

public abstract class MonitorData {
  public String timestamp, nodeName;

  public MonitorData(String nodeName) {
    this.timestamp = TimeUtil.now();
    this.nodeName = new String(nodeName);
  }

  public MonitorData(String timestamp, String nodeName) {
    this.timestamp = new String(timestamp);
    this.nodeName = new String(nodeName);
  }
}
