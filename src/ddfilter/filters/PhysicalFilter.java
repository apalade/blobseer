package filters;

import data.MonitorData;
import data.PhysicalData;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Vector;
import lia.Monitor.Filters.GenericMLFilter;
import lia.Monitor.monitor.eResult;
import lia.Monitor.monitor.monPredicate;
import tables.DatabaseAccess;
import tables.PhysicalTable;
import tables.TableInterface;

class ProcessPhysicalMonData {

  private HashMap<String, Vector<MonitorData>> monitor_data;
  private HashSet<String> initializedTables;
  private HashSet<String> to_process;
  private PhysicalTable physicalTable;

  public ProcessPhysicalMonData(HashSet<String> to_process) {
    monitor_data = new HashMap<String, Vector<MonitorData>>();
    initializedTables = new HashSet<String>();
    DatabaseAccess dba = new DatabaseAccess("/tmp/filterdb.conf");
    physicalTable = new PhysicalTable(dba);
    this.to_process = to_process;
  }

  public Object expressResults() //called periodically
  {
    synchronized (monitor_data) {
      //the vector has to be emptied after sending the results
      Iterator<String> it = monitor_data.keySet().iterator();
      for (Vector<MonitorData> v : monitor_data.values()) {
        for (int i = 0; i < v.size(); ++i) {
          MonitorData m = (MonitorData) v.elementAt(i);
          persist(physicalTable, m);
        }
      }
      monitor_data.clear();
    }
    return null;
  }

  public void processData(MonitorData received_param) {
    if (monitor_data.containsKey(received_param.nodeName)) //the client is already present in the received data
    {
      Vector<MonitorData> data = monitor_data.get(received_param.nodeName);
      data.add(received_param);
    } else // data belong to a new client
    {
      Vector<MonitorData> data = new Vector<MonitorData>();
      data.add(received_param);
      monitor_data.put(received_param.nodeName, data);
    }
  }

  private void persist(TableInterface table, MonitorData m) {
    if (!initializedTables.contains(m.nodeName)) {
      physicalTable.createTable(m.nodeName, to_process);
      initializedTables.add(m.nodeName);
    }
    table.insert(m);
  }
}

/**
 *
 * @author Alexandru Palade <alexandru.palade@loopback.ro>
 */
public class PhysicalFilter extends GenericMLFilter {

  private final static String name = "DDFilter";
  private monPredicate[] predicates = null;
  private static long TIMEOUT = 3000;
  private ProcessPhysicalMonData dataManager;
  private final HashSet<String> to_process;

  public PhysicalFilter(String farmName) {
    super(farmName);
    to_process = new HashSet<String>();
    to_process.add("free");

    dataManager = new ProcessPhysicalMonData(to_process);
    predicates = new monPredicate[]{
              new monPredicate(farmName, "ProviderPhysical", "*", -1, -1, null, null)
            };
  }

  protected MonitorData parseResult(eResult r) {
    HashMap<String, Object> map = new HashMap<String, Object>();
    
    for (int i = 0; i < r.param_name.length; ++i) {
      if (to_process.contains(r.param_name[i])) {
        map.put(r.param_name[i], r.param[i]);
      }
    }
    System.out.println(map);

    PhysicalData data = new PhysicalData(r.FarmName, r.NodeName, map);
    return data;
  }

  @Override
  public String getName() {
    return name;
  }

  @Override
  public long getSleepTime() {
    return TIMEOUT;
  }

  @Override
  public monPredicate[] getFilterPred() {
    return predicates;
  }

  @Override
  public void notifyResult(Object o) {
    if (null == o) {
      return;
    }

    if (o instanceof Vector<?>) {
      Vector<?> v = (Vector<?>) o;
      for (int i = 0; i < v.size(); ++i) {
        notifyResult(v.elementAt(i));
      }

      return;
    }

    if (o instanceof eResult) {
      eResult r = (eResult) o;
      dataManager.processData(parseResult(r));
    }
  }

  @Override
  public Object expressResults() {
    return dataManager.expressResults();
  }
}
