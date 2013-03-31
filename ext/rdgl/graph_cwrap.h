#ifndef RDGL_GRAPH_CWRAP_H
#define RDGL_GRAPH_CWRAP_H

#include "ruby.h"

#ifdef IS_MACOSX
#include <float.h>
#elif IS_LINUX
#include <values.h>
#else
#error "where is DBL_MAX defined here?"
#endif
#define RDGL_GRAPH_INFINITY DBL_MAX

/* only include this when building the rdgl_graph_cwrap.cc binary */
#ifdef __cplusplus
#include "graph.h"
#else
/* just enough of rdgl_graph.h for the C compiler to be happy */
#include <stdint.h>
typedef uint64_t rdgl_vertex_id_t;
typedef uint16_t rdgl_edge_id_t;
typedef float rdgl_edge_weight_t;
typedef rdgl_edge_weight_t rdgl_vertex_distance_t;
#endif

#include "graph_cwrap_structs.h"

#define RDGL_VERTEX_ID_TO_VALUE(x) INT2NUM(x)
#define RDGL_VERTEX_DISTANCE_TO_VALUE(x) rb_float_new(x)
#define RDGL_EDGE_WEIGHT_TO_VALUE(x) rb_float_new(x)

#ifdef __cplusplus
using namespace RDGL;
extern "C" {
#endif

/* AllPairsMap ****************************************************************/
rdgl_all_pairs_map_t * rdgl_apm_wnew(void);
void rdgl_apm_wfree(rdgl_all_pairs_map_t * m);
VALUE rdgl_apm_wextract_ruby_path(rdgl_all_pairs_map_t * m,
                                  rdgl_vertex_t * s,
                                  rdgl_vertex_t * t);
VALUE rdgl_apm_wextract_ruby_all_paths(rdgl_all_pairs_map_t * m);
VALUE rdgl_apm_wto_s(rdgl_all_pairs_map_t * m);

/* Vertex *****************************************************************/
rdgl_vertex_t * rdgl_vertex_wnew(rdgl_vertex_id_t id, rdgl_edge_weight_t d);
void rdgl_vertex_wfree(rdgl_vertex_t * v);
rdgl_vertex_id_t rdgl_vertex_wget_id(rdgl_vertex_t * v);
void rdgl_vertex_wset_id(rdgl_vertex_t * v, rdgl_vertex_id_t id);
rdgl_vertex_distance_t rdgl_vertex_wget_distance(rdgl_vertex_t * v);
void rdgl_vertex_wset_distance(rdgl_vertex_t * v, rdgl_edge_weight_t w);
void rdgl_vertex_wadd_out_edge(rdgl_vertex_t * v, rdgl_edge_t * e);
void rdgl_vertex_wset_back_edge(rdgl_vertex_t * v, rdgl_edge_t * e);
VALUE rdgl_vertex_wget_back_edge(rdgl_vertex_t * v);
VALUE rdgl_vertex_wto_s(rdgl_vertex_t * v);
VALUE rdgl_vertex_winspect(rdgl_vertex_t * v);
void rdgl_vertex_wset_value(rdgl_vertex_t * vt, VALUE vl);
int rdgl_vertex_wequality(rdgl_vertex_t * a, rdgl_vertex_t * b);

/* Edge *******************************************************************/
rdgl_edge_t * rdgl_edge_wnew(rdgl_vertex_t * s, rdgl_vertex_t * t,
                             rdgl_edge_weight_t w, rdgl_edge_id_t id,
                             rdgl_graph_t * g);
void rdgl_edge_wfree(rdgl_edge_t * e);
void rdgl_edge_wrelax(rdgl_edge_t * e);
void rdgl_edge_wreset(rdgl_edge_t * e);
rdgl_edge_weight_t rdgl_edge_wget_weight(rdgl_edge_t * e);
void rdgl_edge_wset_weight(rdgl_edge_t * e, rdgl_edge_weight_t w);
rdgl_edge_id_t rdgl_edge_wget_edge_id(rdgl_edge_t * e);
void rdgl_edge_wset_edge_id(rdgl_edge_t * e, rdgl_edge_id_t id,
                            rdgl_graph_t * graph);
void rdgl_edge_wset_start_vertex(rdgl_edge_t * e, rdgl_vertex_t * v);
void rdgl_edge_wset_end_vertex(rdgl_edge_t * e, rdgl_vertex_t * v);
VALUE rdgl_edge_wto_s(rdgl_edge_t * e);
void rdgl_edge_wset_value(rdgl_edge_t * e, VALUE v);
int rdgl_edge_wequality(rdgl_edge_t * a, rdgl_edge_t * b);

/* Graph **********************************************************************/
rdgl_graph_t * rdgl_graph_wnew(void);
void rdgl_graph_wfree(rdgl_graph_t * g);
void rdgl_graph_wmark(rdgl_graph_t * g);
void rdgl_graph_wadd_edge(rdgl_graph_t * g, rdgl_edge_t * e);
VALUE rdgl_graph_wset_edge_id(rdgl_graph_t * g, rdgl_edge_t * e);
VALUE rdgl_graph_wremove_edge(rdgl_graph_t * g, rdgl_edge_t * e);
VALUE rdgl_graph_wrandom_edge(rdgl_graph_t * g);
void rdgl_graph_wadd_vertex(rdgl_graph_t * g, rdgl_vertex_t * v);
VALUE rdgl_graph_wremove_vertex(rdgl_graph_t * g, rdgl_vertex_t * v);
VALUE rdgl_graph_wrandom_vertex(rdgl_graph_t * g);
void rdgl_graph_wreset(rdgl_graph_t * g, rdgl_vertex_t * s);
int rdgl_graph_wget_num_vertices(rdgl_graph_t * g);
int rdgl_graph_wget_edges(rdgl_graph_t * g);
int rdgl_graph_wrun_dijkstra(rdgl_graph_t * g, rdgl_vertex_t * s);
int rdgl_graph_wrun_bellman_ford(rdgl_graph_t * g, rdgl_vertex_t * s);
int rdgl_graph_wrun_yen(rdgl_graph_t * g,
                        rdgl_vertex_t * s, rdgl_vertex_t * e);
VALUE rdgl_graph_wrun_floyd_warshall(rdgl_graph_t * g);
VALUE rdgl_graph_wto_s(rdgl_graph_t * g);
VALUE rdgl_graph_wextract_path(rdgl_graph_t * g, rdgl_vertex_t * e);
void rdgl_graph_wset_value(rdgl_graph_t * g, VALUE v);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef RDGL_GRAPH_CWRAP_H */

