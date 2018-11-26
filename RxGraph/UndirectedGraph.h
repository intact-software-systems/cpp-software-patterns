#pragma once

#include <boost/graph/adjacency_list.hpp>

namespace RxGraph {

template <typename Vertex, typename Edge>
class UndirectedGraph
{
private:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Vertex, Edge> Graph;

public:
    Graph graph_;
};


template <typename Vertex, typename Edge, typename VertexId>
class GraphBuilder
{
private:
    typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, Vertex, Edge> Graph;
    typedef typename boost::graph_traits<Graph>::vertex_descriptor                            vertex_descriptor;

    typedef std::map<VertexId, vertex_descriptor> VertexIdToGraph;
    typedef std::map<vertex_descriptor, VertexId> GraphToVertexId;

public:

    GraphBuilder* AddVertex(VertexId id, Vertex vertex)
    {

        //graph_.

        return this;
    }


public:
    Graph           graph_;
    VertexIdToGraph vertexIdToGraph_;
    GraphToVertexId graphToVertexId_;
};

}
