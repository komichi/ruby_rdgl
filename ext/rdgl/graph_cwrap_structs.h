#ifndef RDGL_GRAPH_CWRAP_STRUCTS
#define RDGL_GRAPH_CWRAP_STRUCTS

#ifdef __cplusplus

namespace RDGL {

class Graph;
class Vertex;
class Edge;
class AllPairsMap;

extern "C"
{
#endif

// this maps to a RDGL::Graph
typedef struct
{
#ifdef __cplusplus
  RDGL::Graph * graph; // i.e., Graph * g;
#else
  void * graph;
#endif
} rdgl_graph_t;

// this maps to a Vertex
typedef struct
{
#ifdef __cplusplus
  RDGL::Vertex * vertex; // i.e., Vertex * v;
#else
  void * vertex;
#endif
} rdgl_vertex_t;

// this maps to a Edge
typedef struct
{
#ifdef __cplusplus
  RDGL::Edge * edge; // i.e., Edge * e;
#else
  void * edge;
#endif
} rdgl_edge_t;

// this maps to a AllPairsMap
typedef struct
{
#ifdef __cplusplus
  RDGL::AllPairsMap * map; // i.e., Graph * g;
#else
  void * map;
#endif
} rdgl_all_pairs_map_t;

#ifdef __cplusplus
} /* extern "C" { ... } */
} /* namespace RDGL { ... } */
#endif

#endif

