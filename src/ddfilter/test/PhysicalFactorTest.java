package test;

import filters.InternalFilter;
import filters.PhysicalFilter;
import java.util.Vector;
import junit.framework.*;
import lia.Monitor.monitor.eResult;

/**
 *
 * @author Alexandru Palade <alexandru.palade@loopback.ro>
 */
public class PhysicalFactorTest extends TestCase {

  private final String farmName = "blobseerfarm";
  private Vector<eResult> results = new Vector<eResult>();

  @Override
  protected void setUp() {
    final String clusterName = "ProviderPhysical";
    final String nodeNames[] = {"127.0.0.1", "127.0.0.2"};
    final String module = "modulename";
    final String param_names[][] = {
      {"free", "unknown"},
      {"free", "unknown"},
      {"free", "unknown"}
    };
    final Object param_values[][] = {
      {0.7, 0},
      {0.2, 0},
      {0.3, 0}
    };

    for (int i = 0; i < nodeNames.length; ++i) {
      for (int j = 0; j < param_names.length; ++j) {
        eResult e = new eResult(farmName, clusterName, nodeNames[i], module, param_names[j]);
        e.param = param_values[j];
        results.add(e);
      }
    }

    for (int i = 0; i < results.size(); ++i) {
      //System.out.println(results.elementAt(i));
    }
  }

  @Override
  protected void tearDown() {
  }

  public void testFilter() {
    PhysicalFilter filter = new PhysicalFilter(farmName);
    filter.notifyResult(results);
    filter.expressResults();
  }
}
