#include <valhalla/baldr/graphid.h>
#include <valhalla/baldr/signinfo.h>
#include <valhalla/mjolnir/uniquenames.h>

#include "test.h"

#include "mjolnir/graphbuilder.h"

using namespace std;
using namespace valhalla::mjolnir;
using valhalla::mjolnir::GraphBuilder;

namespace {

class test_graph_builder : public GraphBuilder {
 public:
  using GraphBuilder::CreateExitSignInfoList;
};

void ExitToTest() {
  GraphId nodeid(1111, 2, 1);
  Node node;
  OSMWay way{};
  std::unordered_map<GraphId, std::string> map_ref;
  std::unordered_map<GraphId, std::string> map_name;
  std::unordered_map<GraphId, std::string> map_exit_to;
  OSMData osmdata{};

  node.set_exit_to(true);

  map_exit_to[nodeid] = "US 11;To I 81;Carlisle;Harrisburg";

  std::vector<SignInfo> exitsigns;
  exitsigns = test_graph_builder::CreateExitSignInfoList(nodeid, node, way, osmdata, map_ref, map_exit_to, map_name);

  if (exitsigns.size() == 4) {
    for (auto& exitsign : exitsigns) {
      if (exitsign.type() != Sign::Type::kExitToward)
        throw std::runtime_error("US 11;To I 81;Carlisle;Harrisburg types are not all Toward");
    }

    if (exitsigns[0].text() != "US 11" && exitsigns[1].text() != "I 81" &&
        exitsigns[2].text() != "Carlisle" && exitsigns[3].text() != "Harrisburg")
      throw std::runtime_error("Exitsign text is bad for US 11;To I 81;Carlisle;Harrisburg.");
  }
  else throw std::runtime_error("US 11/To I 81/Carlisle/Harrisburg failed to be parsed.  " + exitsigns.size() );

  exitsigns.clear();
  map_exit_to[nodeid] = "US 11;Toward I 81;Carlisle;Harrisburg";

  exitsigns = test_graph_builder::CreateExitSignInfoList(nodeid, node, way, osmdata, map_ref, map_exit_to, map_name);

  if (exitsigns.size() == 4) {
    for (auto& exitsign : exitsigns) {
      if (exitsign.type() != Sign::Type::kExitToward)
        throw std::runtime_error("US 11;Toward I 81;Carlisle;Harrisburg types are not all Toward");
    }
    if (exitsigns[0].text() != "US 11" && exitsigns[1].text() != "I 81" &&
        exitsigns[2].text() != "Carlisle" && exitsigns[3].text() != "Harrisburg")
      throw std::runtime_error("Exitsign text is bad for US 11;To I 81;Carlisle;Harrisburg.");
  }
  else throw std::runtime_error("US 11;Toward I 81;Carlisle;Harrisburg failed to be parsed.");

  exitsigns.clear();
  map_exit_to[nodeid] = "I 95 To I 695";

  exitsigns = test_graph_builder::CreateExitSignInfoList(nodeid, node, way, osmdata, map_ref, map_exit_to, map_name);

  if (exitsigns.size() == 2) {
     if (exitsigns[0].type() != Sign::Type::kExitBranch)
       throw std::runtime_error("I 95 should be a branch.");

     if (exitsigns[1].type() != Sign::Type::kExitToward)
       throw std::runtime_error("I 695 should be a toward.");

     if (exitsigns[0].text() != "I 95" && exitsigns[1].text() != "I 695")
       throw std::runtime_error("Exitsign text is bad for I 95 To I 695");
  }
  else throw std::runtime_error("I 95 To I 695 failed to be parsed.");

  exitsigns.clear();
  map_exit_to[nodeid] = "I 495 Toward I 270";

  exitsigns = test_graph_builder::CreateExitSignInfoList(nodeid, node, way, osmdata, map_ref, map_exit_to, map_name);

  if (exitsigns.size() == 2) {
    if (exitsigns[0].type() != Sign::Type::kExitBranch)
      throw std::runtime_error("I 495 should be a branch.");

    if (exitsigns[1].type() != Sign::Type::kExitToward)
      throw std::runtime_error("I 270 should be a toward.");

    if (exitsigns[0].text() != "I 495" && exitsigns[1].text() != "I 270")
      throw std::runtime_error("Exitsign text is bad for I 495 Toward I 270");
  }
  else throw std::runtime_error("I 495 Toward I 270 failed to be parsed.");

  exitsigns.clear();
  map_exit_to[nodeid] = "I 495 Toward I 270 To I 95";//default to toward.  Punt on parsing.

  exitsigns = test_graph_builder::CreateExitSignInfoList(nodeid, node, way, osmdata, map_ref, map_exit_to, map_name);

  if (exitsigns.size() == 1) {
    if (exitsigns[0].type() != Sign::Type::kExitToward)
      throw std::runtime_error("I 495 Toward I 270 To I 95 should be a toward.");

    if (exitsigns[0].text() != "I 495 Toward I 270 To I 95")
      throw std::runtime_error("Exitsign text is bad for I 495 Toward I 270 To I 95");
  }
  else throw std::runtime_error("I 495 Toward I 270 To I 95 failed to be parsed.");
}

}

int main() {
  test::suite suite("signinfo");

  // Test exit to logic.
  suite.test(TEST_CASE(ExitToTest));

  return suite.tear_down();
}
