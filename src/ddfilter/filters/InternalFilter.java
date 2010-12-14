package filters;
/**
 * Filter based on Alexandra Carpen-Amarie's and Michi's filter code
 */
import data.MonitorData;
import data.ProviderReadData;
import data.ProviderWriteData;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.Vector;
import lia.Monitor.Filters.GenericMLFilter;
import lia.Monitor.monitor.eResult;
import lia.Monitor.monitor.monPredicate;
import tables.DatabaseAccess;
import tables.ProviderReadTable;
import tables.ProviderWriteTable;
import tables.TableInterface;

class ProcessInternalMonData {
  private HashMap<String, Vector<MonitorData>> monitor_data;
  private HashSet<String> initializedTables;
  private ProviderReadTable readTable;
  private ProviderWriteTable writeTable;

  public ProcessInternalMonData() {
    monitor_data = new HashMap<String, Vector<MonitorData>>();
    initializedTables = new HashSet<String>();
    DatabaseAccess dba = new DatabaseAccess("/tmp/filterdb.conf");
    readTable = new ProviderReadTable(dba);
    writeTable = new ProviderWriteTable(dba);
  }

  public Object expressResults() //called periodically
  {
    synchronized (monitor_data) {
      //the vector has to be emptied after sending the results
      Iterator<String> it = monitor_data.keySet().iterator();
      for (Vector<MonitorData> v : monitor_data.values()) {
        for (int i = 0; i < v.size(); ++i) {
          MonitorData m = (MonitorData) v.elementAt(i);

          if (m instanceof ProviderReadData) {
            persist(readTable, m);
          } else if (m instanceof ProviderWriteData) {
            persist(writeTable, m);
          }
        }
      }
      monitor_data.clear();
    }
    return null;
  }

  public void processData(MonitorData received_param) {
    if (null == received_param) {
      return;
    }
    
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
      readTable.createTable(m.nodeName);
      writeTable.createTable(m.nodeName);
      initializedTables.add(m.nodeName);
    }
    table.insert(m);
  }
}

/**
 *
 * @author Alexandru Palade <alexandru.palade@loopback.ro>
 */
public class InternalFilter extends GenericMLFilter {

  private final static String name = "DDFilter";
  private monPredicate[] predicates = null;
  private static long TIMEOUT = 3000;
  private ProcessInternalMonData dataManager;

  public InternalFilter(String farmName) {
    super(farmName);
    dataManager = new ProcessInternalMonData();
    predicates = new monPredicate[]{
      new monPredicate(farmName, "Blob_IO", "*", -1, -1, null, null)
    };
  }

  protected MonitorData parseResult(eResult r) {
    String sParam = r.param_name[0];
    System.err.println("A ajuns rezultatul: " + r.param_name[0] + " cu valoarea" + r.param[0]);
    String[] IO_arr = ((String) r.param[0]).split("#");

    if (sParam.equals("provider_write")) {
      ProviderWriteData prov = new ProviderWriteData(r.FarmName, r.NodeName, IO_arr);
      return prov;
    }

    if (sParam.equals("provider_read")) {
      ProviderReadData prov = new ProviderReadData(r.FarmName, r.NodeName, IO_arr);
      return prov;
    }

    return null;
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
