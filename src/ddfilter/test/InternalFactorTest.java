package test;

import filters.InternalFilter;
import java.util.Vector;
import junit.framework.*;
import lia.Monitor.monitor.eResult;

/**
 *
 * @author Alexandru Palade <alexandru.palade@loopback.ro>
 */
public class InternalFactorTest extends TestCase {

  private final String farmName = "blobseerfarm";
  private Vector<eResult> results = new Vector<eResult>();

  @Override
  protected void setUp() {
    final String clusterName = "Blob_IO";
    final String nodeNames[] = {"127.0.0.1", "127.0.0.2"};
    final String module = "modulename";
    final Integer blob_ids[] = {1, 2, 3};
    final Integer page_indexes[] = {4, 5};
    final String watermarks[] = {"watermark"};
    final Integer page_sizes[] = {64, 128};
    final String client_ids[] = {"191.192.1.1"};
    final String provider_timestamps[] = {"1277724665166"};
    final String param_names[][] = {{"provider_write"}, {"provider_read"}, {"unknown"}};
    // Param_name: "page_index#blob_id#watermark#page_size#client_id#provider_timestamp"

    for (int l = 0; l < param_names.length; ++l) {
      for (int i = 0; i < nodeNames.length; ++i) {
        for (int k = 0; k < page_indexes.length; ++k) {
          for (int j = 0; j < blob_ids.length; ++j) {
            for (int p = 0; p < watermarks.length; ++p) {
              for (int m = 0; m < page_sizes.length; ++m) {
                for (int n = 0; n < client_ids.length; ++n) {
                  for (int q = 0; q < provider_timestamps.length; ++q) {
                    String param_values[] = {""
                      + page_indexes[k] + "#" + blob_ids[j] + "#"
                      + watermarks[p] + "#" + page_sizes[m] + "#"
                      + client_ids[n] + "#" + provider_timestamps[q]
                    };
                    eResult e = new eResult(farmName, clusterName, nodeNames[i], module, param_names[l]);
                    e.param = param_values;
                    results.add(e);
                  }

                }
              }
            }
          }
        }
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
    InternalFilter filter = new InternalFilter(farmName);
    filter.notifyResult(results);
    filter.expressResults();

  }
}
