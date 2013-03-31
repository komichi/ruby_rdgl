#ifndef RDGL_GRAPH_H
#define RDGL_GRAPH_H

#include "graph_cwrap_structs.h"
#include "graph_general.h"

#include <vector>
#include <set>
#include <map>
#include <queue>
#include <iostream>
#include <iomanip>
#include <string>
#include <stdlib.h>

#define PH_CMP std::deref_greater<T *>()
#include "pointer_heap.h"
#include "pointer_set.h"

#include <stdint.h>

// from_graph(...)
#define STDIN 0
#include <sys/types.h>
#include <regex.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#include <limits>
//const double dMax = std::numeric_limits<double>::max();
#define MAX_DISTANCE std::numeric_limits<double>::max()

namespace RDGL {

typedef union
{
  uint64_t u64;
  struct
  {
    uint32_t upper;
    uint32_t lower;
  } u32;
} u64_to_u32_union;
typedef uint64_t rdgl_vertex_id_t;
typedef uint16_t rdgl_edge_id_t;
typedef float rdgl_edge_weight_t;
typedef rdgl_edge_weight_t rdgl_vertex_distance_t;

/*
  void initialize_edge_id(rdgl_edge_id_t & id, Edge & s, Edge & e);
*/


class Edge; // dependency: EdgeSet -> Edge -> Vertex
class EdgeSet;
class Graph;

/******************************************************************************/
/* Vertex *********************************************************************/
/******************************************************************************/

class Vertex
{
  public:
    // methods
    Vertex(void);
    Vertex(rdgl_vertex_id_t id, rdgl_vertex_distance_t distance);
    ~Vertex(void);
    Vertex(const Vertex & v);
    void reset(void);
    void add_out_edge(Edge & e);
    bool operator<(const Vertex & b) const;
    bool operator>(const Vertex & b) const;
    bool operator==(const Vertex & b) const;
    bool operator!=(const Vertex & b) const;
    bool operator()(const Vertex * a, Vertex * b) const;
    friend std::ostream & operator<<(std::ostream & os, Vertex & v);
    // data
    rdgl_vertex_id_t id;
    rdgl_vertex_distance_t distance;
    EdgeSet * out_edges;
    //Vertex * successor, * predecessor;
    Edge * forward_edge, * back_edge;
#ifdef RUBY_H
    VALUE value; // a pointer back to the VALUE enclosing us (for marking / returning in ruby)
#endif
};

bool vertex_id_lt_cmp(const Vertex * a, const Vertex * b);

/******************************************************************************/
/* VertexSet **************************************************************/
/******************************************************************************/

class VertexSet : public PointerSet<Vertex>
{
  public:
    // we need this because the superclass uses '<' to perform the comparison
    // which, in turn, compares values; Vertex::operator== only compares
    // vertex ids; perhaps not the most intuitive design, but no other way
    // seems more intuitive at this point w.r.t. "what you'd expect" == to do
    Vertex * find(Vertex & v);
    Vertex * random(void);
};

/******************************************************************************/
/* VertexList *************************************************************/
/******************************************************************************/

class VertexList : public std::vector<Vertex *>
{
  public:
    VertexList(void) : std::vector<Vertex *>() { ; }
    VertexList(size_t n) : std::vector<Vertex *>(n) { ; }
    void prepend(Vertex & v);
    void insert(Vertex & v);
    void append(Vertex & v);
    Vertex * remove_smallest(void);
    friend std::ostream & operator<<(std::ostream & os, VertexList & l);
};

/******************************************************************************/
/* VertexHeap *************************************************************/
/******************************************************************************/

typedef PointerHeap<Vertex> VertexHeap;

/******************************************************************************/
/* VertexPriorityQueue ****************************************************/
/******************************************************************************/
/*
#define RDGL_VPQ_CTN_CLASS VertexList
#define RDGL_VPQ_CLASS std::priority_queue<Vertex *, \
                                           RDGL_VPQ_CTN_CLASS, \
                                           std::deref_greater<Vertex *> >
class VertexPriorityQueue : public RDGL_VPQ_CLASS
{
  public:
    VertexPriorityQueue(void) : RDGL_VPQ_CLASS() { ; }
    Vertex * push(Vertex & v);
    Vertex * pop(Vertex & v);
    Vertex * min(Vertex & v);
    void decrease_key(Vertex & v, rdgl_vertex_distance_t d);
    friend std::ostream & operator<<(std::ostream & os,
                                     VertexPriorityQueue & l);
};
*/

/******************************************************************************/
/* Matrix *****************************************************************/
/******************************************************************************/

typedef uint32_t rdgl_matrix_id_t;
template <typename T>
class Matrix
{
  public:
    Matrix(rdgl_matrix_id_t s, T i);
    Matrix(rdgl_matrix_id_t s);
    ~Matrix(void);
    T * operator[](rdgl_matrix_id_t i);
  private:
    rdgl_matrix_id_t size;
    T **matrix; //declares matrix (double pointer)
};

template <typename T>
Matrix<T>::Matrix(rdgl_matrix_id_t s)
{
  size = s;
  matrix = new T * [size]; // allocates an array of pointers
  for (rdgl_matrix_id_t i=0;i<size;i++)
    matrix[i] = new T [size]; // allocates an array of size to each
}

template <typename T>
Matrix<T>::Matrix(rdgl_matrix_id_t s, T init)
{
  size = s;
  matrix = new T * [size]; // allocates an array of pointers
  for (rdgl_matrix_id_t i=0;i<size;i++)
  {
    matrix[i] = new T [size]; // allocates an array of size to each
    for (rdgl_matrix_id_t j=0; j<size; j++)
      matrix[i][j] = init; // initialize
  }
}

template <typename T>
Matrix<T>::~Matrix(void)
{
  //destroy the matrix
  for (rdgl_matrix_id_t i=0;i<size;i++)
  {
    delete [] matrix[i]; //deletes each array within matrix
  }
  delete [] matrix; //deletes the array of pointers
}

template <typename T>
T * Matrix<T>::operator[](rdgl_matrix_id_t i)
{
  if (i > size) return NULL;
  return matrix[i];
}

/******************************************************************************/
/* Edge *******************************************************************/
/******************************************************************************/

class Edge
{
  public:
    // methods
    Edge(Vertex & s, Vertex & e, rdgl_edge_weight_t w=0.0, rdgl_edge_id_t id=0, Graph * g=NULL);
    Edge(Vertex * s=NULL, Vertex * e=NULL, rdgl_edge_weight_t w=0.0, rdgl_edge_id_t id=0, Graph * g=NULL);
    Edge(const Edge & e);
    ~Edge(void);
    void initialize(Vertex * s=NULL, Vertex * e=NULL,
                    rdgl_edge_weight_t w=0.0, rdgl_edge_id_t id=0,
                    Graph * g=NULL);
    void reset(void);
    bool relax(void);
    bool operator=(const Edge & b);
    bool operator<(const Edge & b) const;
    bool operator>(const Edge & b) const;
    bool operator==(const Edge & b) const;
    bool operator!=(const Edge & b) const;
    bool operator()(const Edge * a, Edge * b) const;
    friend std::ostream & operator<<(std::ostream & os, Edge & e);
    // data
    Vertex * start_vertex,
               * end_vertex;
    rdgl_edge_weight_t weight;
    rdgl_edge_id_t edge_id;
#ifdef RUBY_H
    VALUE value; // a pointer back to the VALUE enclosing us (for marking / returning in ruby)
#endif
};

bool rdgl_edge_lt_cmp(const Edge * a, const Edge * b);

/******************************************************************************/
/* EdgeSet ****************************************************************/
/******************************************************************************/

class EdgeSet : public PointerSet<Edge>
{
  public:
    EdgeSet(void) : PointerSet<Edge>() { ; }
    Edge * find(Edge & e);
    Edge * random(void);
};

/******************************************************************************/
/* Path *******************************************************************/
/******************************************************************************/

class Path : public std::vector<Edge *>
{
  public:
    void prepend(Edge & e);
    void append(Edge & e);
    size_t length(void) { return this->size(); }
    bool operator==(Path & p);
    friend std::ostream & operator<<(std::ostream & os, Path & p);
};

/******************************************************************************/
/* PathSet ****************************************************************/
/******************************************************************************/

class PathSet : public std::vector<Path *>
{
  public:
    void prepend(Path & p);
    void append(Path & p);
    void insert(Path & p);
    friend std::ostream & operator<<(std::ostream & os, PathSet & p);
};

/******************************************************************************/
/* Graph ******************************************************************/
/******************************************************************************/

#define SG_SP_ALGO_TRAD_DIJKSTRA "Traditional Dijkstra"
#define SG_SP_ALGO_DIJKSTRA      "Dijkstra"
#define SG_SP_ALGO_BELLMAN_FORD  "Bellman-Ford"
#define SG_SP_ALGO_YEN           "Yen"

class AllPairsMap;

// NOTE: we do not allow multigraphs ... who says so?
// FIXME: find (for both vertices and edges) is linear search-based despite the
//        fact that the base class is std::set; the problem is that set uses
//        operator== which compares all values instead of just ids / endpoints
class Graph
{
  public:
    // methods
    Graph(void);
    Graph(Graph & g);
    ~Graph(void);
    bool from_file(const char * file_name);
    bool from_inet_file(const char * file_name);
    bool from_skitter_file(const char * file_name);
    void reset(void); // resets vertex distances and predecessors (only)
    void reset(Vertex * start);
    void reset(Vertex & v);
    Vertex * add_vertex(Vertex & v);
    Vertex * remove_vertex(Vertex & v);
    Vertex * random_vertex(void);
    Edge * add_edge(rdgl_vertex_id_t v1, rdgl_vertex_id_t v2,
                        rdgl_edge_weight_t w, rdgl_edge_id_t id=1);
    Edge * add_edge(Edge & e);
    Edge * remove_edge(Edge & e);
    Edge * random_edge(void);
    bool run_dijkstra(Vertex & start);
    bool run_priority_queue_dijkstra(Vertex & start);
    bool run_stl_heap_dijkstra(Vertex & start);
    bool run_traditional_dijkstra(Vertex & start);
    bool run_bellman_ford(Vertex & start);
    AllPairsMap * run_floyd_warshall(void);
#ifdef RUBY_H
    VALUE run_ruby_floyd_warshall(void);
#endif
    size_t num_vertices(void);
    size_t order(void);
    size_t num_edges(void);
    size_t size(void);
    PathSet * run_yen(Vertex & start, Vertex & end);
    Path * extract_path(Vertex & end);
    PathSet * extract_all_paths(void);
    // data
    VertexSet * vertices;
    EdgeSet * edges;
    void initialize_edge_id(Vertex & s, Vertex & d, rdgl_edge_id_t & i);
#ifdef RUBY_H
    VALUE set_edge_id(Edge & edge);
#else
    void set_edge_id(Edge & edge);
#endif
    const char * file_name,
               * sp_algorithm;
#ifdef RUBY_H
    void ruby_gc_mark(void); // mark as still in use
    VALUE extract_ruby_path(Vertex & end);
    VALUE value; // a pointer back to the VALUE enclosing us (for marking / returning in ruby)
#endif
    friend std::ostream & operator<<(std::ostream & os, Graph & g);
};

/******************************************************************************/
/* AllPairsMap ************************************************************/
/******************************************************************************/

typedef rdgl_matrix_id_t rdgl_all_pairs_map_id_t;
class AllPairsMap
{
  public:
    AllPairsMap(Graph & g);
    rdgl_edge_weight_t get_distance(rdgl_all_pairs_map_id_t start,
                                    rdgl_all_pairs_map_id_t v);
    rdgl_edge_weight_t d(rdgl_all_pairs_map_id_t start,
                         rdgl_all_pairs_map_id_t v);
    rdgl_all_pairs_map_id_t get_predecessor(rdgl_all_pairs_map_id_t start,
                                            rdgl_all_pairs_map_id_t v);
    rdgl_all_pairs_map_id_t p(rdgl_all_pairs_map_id_t start,
                              rdgl_all_pairs_map_id_t v);
    rdgl_edge_weight_t set_distance(rdgl_all_pairs_map_id_t start,
                                    rdgl_all_pairs_map_id_t v,
                                    rdgl_edge_weight_t w);
    rdgl_all_pairs_map_id_t set_predecessor(rdgl_all_pairs_map_id_t start,
                                            rdgl_all_pairs_map_id_t v,
                                            rdgl_all_pairs_map_id_t p);
    rdgl_all_pairs_map_id_t get_id_from_vertex(Vertex & v);
    Vertex * get_vertex_from_id(rdgl_all_pairs_map_id_t);
    friend AllPairsMap * Graph::run_floyd_warshall(void);
    Path * extract_path(Vertex & start, Vertex & end);
    PathSet * extract_all_paths(void);
    VALUE extract_all_ruby_paths(void);
    VALUE extract_ruby_path(Vertex & start, Vertex & end);
    VALUE value;
    friend std::ostream & operator<<(std::ostream & os, AllPairsMap & m);
  private:
    void set_vertex_id(Vertex & v, rdgl_all_pairs_map_id_t id);
    void set_graph(Graph & g);
    VertexList * vertices;
    Matrix<rdgl_edge_weight_t> * distances;
    Matrix<rdgl_all_pairs_map_id_t> * predecessors;
    std::map<Vertex *, rdgl_all_pairs_map_id_t> * ptr2id;
    Graph * graph;
};

}

#endif /* #ifndef RDGL_GRAPH_H */

