// Vsevolod Ivanov

#include "boost/graph/adjacency_list.hpp"
#include <boost/graph/subgraph.hpp>
#include <boost/graph/graphviz.hpp>

using namespace std;
using namespace boost;

typedef subgraph<adjacency_list<
    vecS, vecS, undirectedS,
    property<vertex_index_t, size_t>,
    property<edge_index_t, size_t>
>> Graph;


void gprint(Graph& graph)
{
    auto vs = boost::vertices(graph);
    for (auto vit = vs.first; vit != vs.second; ++vit)
    {
        cout << "V=" << *vit << " EDGES=";
        auto neighbors = boost::adjacent_vertices(*vit, graph);
        for (auto nit = neighbors.first; nit != neighbors.second; ++nit)
            std::cout << "{" << *vit << "," << *nit << "}" << " ";
        cout << endl;
    }
}

int main()
{
    const int CONTINENTS = 3;
    enum { NorthAmerica, CentralAmercia};
    enum {Canada, USA, Mexico};
    cout << "Canada=" << Canada <<
            " USA=" << USA <<
            " Mexico=" << Mexico <<
    endl;

    Graph G0(CONTINENTS);

    Graph& G1 = G0.create_subgraph();
    Graph& G2 = G0.create_subgraph();

    add_vertex(Canada, G1);
    add_vertex(USA, G2);
    add_vertex(Mexico, G2);

    add_edge(Canada, USA, G0);
    add_edge(USA, Mexico, G0);

    cout << "G0(world):" << endl; gprint(G0);
    cout << "G1(continent):" << endl; gprint(G1);
    cout << "G2(continent):" << endl; gprint(G2);

    return 0;
}
