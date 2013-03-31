#include <iostream>
#include <sstream>
#include <vector>

#include "graph.h"
#include "graph_cwrap.h"

using namespace RDGL;

// all of this must have C linkage to link within Ruby
extern "C"
{

/******************************************************************************/
/* AllPairsMap ****************************************************************/
/******************************************************************************/

// This doesn't actually do anything but erect an empty struct
// AllPairsMap.new is never meant to be called -- Graph::run_floyd...
// is called and it calls the AllPairs(Graph & g) constructor
// which, in turn, calls this guy here, and then fills in the structure.
rdgl_all_pairs_map_t *
rdgl_apm_wnew(void)
{
  rdgl_all_pairs_map_t * m = NULL;
  m = (rdgl_all_pairs_map_t *) malloc(sizeof(rdgl_all_pairs_map_t));
  m->map = NULL;
  return m;
}

void
rdgl_apm_wfree(rdgl_all_pairs_map_t * m)
{
  if (m && m->map) delete m->map;
  if (m) free(m);
}

VALUE
rdgl_apm_wextract_ruby_path(rdgl_all_pairs_map_t * m,
                            rdgl_vertex_t * s,
                            rdgl_vertex_t * t)
{
  return m->map->extract_ruby_path(*(s->vertex), *(t->vertex));
}

VALUE
rdgl_apm_wextract_ruby_all_paths(rdgl_all_pairs_map_t * m)
{
  return m->map->extract_all_ruby_paths();
}

VALUE
rdgl_apm_wto_s(rdgl_all_pairs_map_t * m)
{
  std::stringstream s;
  if (m && m->map)
  {
    s << *(m->map);
    return rb_str_new2(s.str().c_str());
  }
  LOG_ERROR("null pointer!");
  return Qnil;
}

/*******************************************************************************
 ** Vertex *********************************************************************
 ******************************************************************************/

rdgl_vertex_t *
rdgl_vertex_wnew(rdgl_vertex_id_t id, rdgl_edge_weight_t d)
{
  rdgl_vertex_t * v = (rdgl_vertex_t *) malloc(sizeof(rdgl_vertex_t));
  v->vertex = new RDGL::Vertex(id, d);
  return v;
}

void
rdgl_vertex_wfree(rdgl_vertex_t * v)
{
  LOG_DEBUG("v is (0x%lx)", v)
  if (v && v->vertex)
  {
    delete v->vertex;
    free(v);
  }
  else
    LOG_ERROR("null pointer!");
}

rdgl_vertex_id_t
rdgl_vertex_wget_id(rdgl_vertex_t * v)
{
  if (v && v->vertex)
    return v->vertex->id;
  LOG_ERROR("null pointer!");
  return -1;
}

void
rdgl_vertex_wset_vertex_id(rdgl_vertex_t * v, rdgl_vertex_id_t id)
{
  if (v && v->vertex)
    v->vertex->id = id;
  else
    LOG_ERROR("null pointer!");
}

rdgl_vertex_distance_t
rdgl_vertex_wget_distance(rdgl_vertex_t * v)
{
  if (v && v->vertex)
    return v->vertex->distance;
  LOG_ERROR("null pointer!");
  return -1;
}

void
rdgl_vertex_wset_distance(rdgl_vertex_t * v, rdgl_edge_weight_t d)
{
  if (v && v->vertex)
    v->vertex->distance = d;
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_vertex_wadd_out_edge(rdgl_vertex_t * v, rdgl_edge_t * e)
{
  if (v && v->vertex && e && e->edge)
    v->vertex->add_out_edge(*(e->edge));
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_vertex_wset_back_edge(rdgl_vertex_t * v, rdgl_edge_t * e)
{
  if (v && v->vertex && e && e->edge)
    v->vertex->back_edge = e->edge;
  else
    LOG_ERROR("null pointer!");
}

VALUE
rdgl_vertex_wget_back_edge(rdgl_vertex_t * v)
{
  if (v && v->vertex && v->vertex->back_edge)
    return v->vertex->back_edge->value;
  else
    return Qnil;
}

void
rdgl_vertex_wset_id(rdgl_vertex_t * v, rdgl_vertex_id_t id)
{
  if (v && v->vertex)
    v->vertex->id = id;
  else
    LOG_ERROR("null pointer!");
}

VALUE
rdgl_vertex_wto_s(rdgl_vertex_t * v)
{
  std::stringstream s;
  if (v && v->vertex)
  {
    s << *(v->vertex);
    return rb_str_new2(s.str().c_str());
  }
  LOG_ERROR("null pointer!");
  return Qnil;
}

VALUE
rdgl_vertex_winspect(rdgl_vertex_t * v)
{
  std::stringstream s;
  if (v && v->vertex)
  {
    s << "[rp=" << v->vertex << ", cp=" << (void *) v->vertex->value << "]";
    return rb_str_new2(s.str().c_str());
  }
  LOG_ERROR("null pointer!");
  return Qnil;
}

void
rdgl_vertex_wset_value(rdgl_vertex_t * vt, VALUE vl)
{
  vt->vertex->value = vl;
}

int
rdgl_vertex_wequality(rdgl_vertex_t * a, rdgl_vertex_t * b)
{
  if (a && b && a->vertex && b->vertex)
    return (*(a->vertex) == *(b->vertex)) ? 1 : 0;
  return -1;
}

/*******************************************************************************
 ** Edge ***********************************************************************
 ******************************************************************************/

rdgl_edge_t *
rdgl_edge_wnew(rdgl_vertex_t * s,
               rdgl_vertex_t * t,
               rdgl_edge_weight_t w,
               rdgl_edge_id_t id,
               rdgl_graph_t * g)
{
  rdgl_edge_t * e = NULL;
  RDGL::Graph * graph = (g && g->graph) ? g->graph : NULL;
  if (s && s->vertex && t && t->vertex)
  {
    e = (rdgl_edge_t *) malloc(sizeof(rdgl_edge_t));
    e->edge = new RDGL::Edge(s->vertex, t->vertex, w, id, graph);
  }
  else
    LOG_ERROR("null pointer!");
  return e;
}

void
rdgl_edge_wfree(rdgl_edge_t * e)
{
  LOG_DEBUG("e is 0x%lx", e)
  if (e && e->edge)
  {
    delete e->edge;
    free(e);
  }
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_edge_wrelax(rdgl_edge_t * e)
{
  if (e && e->edge)
    e->edge->relax();
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_edge_wreset(rdgl_edge_t * e)
{
  if (e && e->edge)
    e->edge->reset();
  else
    LOG_ERROR("null pointer!");
}

rdgl_edge_weight_t
rdgl_edge_wget_weight(rdgl_edge_t * e)
{
  if (e && e->edge)
    return e->edge->weight;
  LOG_ERROR("null pointer!");
  return 0;
}

void
rdgl_edge_wset_weight(rdgl_edge_t * e, rdgl_edge_weight_t w)
{
  if (e && e->edge)
    e->edge->weight = w;
  else
    LOG_ERROR("null pointer!");
}

rdgl_edge_id_t
rdgl_edge_wget_edge_id(rdgl_edge_t * e)
{
  if (e && e->edge)
    return e->edge->edge_id;
  LOG_ERROR("null pointer!");
  return 0;
}

void
rdgl_edge_wset_edge_id(rdgl_edge_t * e, rdgl_edge_id_t id, rdgl_graph_t * g)
{
  if (e && e->edge)
  {
    if (g && g->graph)
    {
      e->edge->edge_id = 0;
      g->graph->initialize_edge_id(*e->edge->start_vertex,
                                   *e->edge->end_vertex,
                                   e->edge->edge_id);
    }
    else
      e->edge->edge_id = id;
  }
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_edge_wset_start_vertex(rdgl_edge_t * e, rdgl_vertex_t * v)
{
  if (e && e->edge && v && v->vertex)
    // FIXME: free or unref the existing vertex?
    e->edge->start_vertex = v->vertex;
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_edge_wset_end_vertex(rdgl_edge_t * e, rdgl_vertex_t * v)
{
  if (e && e->edge && v && v->vertex)
    // FIXME: free or unref the existing vertex?
    e->edge->end_vertex = v->vertex;
  else
    LOG_ERROR("null pointer!");
}

VALUE
rdgl_edge_wto_s(rdgl_edge_t * e)
{
  std::stringstream s;
  if (e && e->edge)
  {
    s << *(e->edge);
    return rb_str_new2(s.str().c_str());
  }
  LOG_ERROR("null pointer!");
  return Qnil;
}

void
rdgl_edge_wset_value(rdgl_edge_t * e, VALUE v)
{
  e->edge->value = v;
}

int
rdgl_edge_wequality(rdgl_edge_t * a, rdgl_edge_t * b)
{
  if (a && b && a->edge && b->edge)
    return (*(a->edge) == *(b->edge)) ? 1 : 0;
  return -1;
}

/****************************************************************************** 
 ** Graph *********************************************************************
 ******************************************************************************/

// make a new graph
rdgl_graph_t *
rdgl_graph_wnew(void)
{
  rdgl_graph_t * g = (rdgl_graph_t *) malloc(sizeof(rdgl_graph_t));
  g->graph = new RDGL::Graph;
  return g;
}

// destroy an existing graph
void
rdgl_graph_wfree(rdgl_graph_t * g)
{
  LOG_DEBUG("g is 0x%lx", g)
  if (g && g->graph)
  {
    delete g->graph;
    free(g);
  }
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_graph_wmark(rdgl_graph_t * g)
{
  if (g && g->graph)
    g->graph->ruby_gc_mark();
  else
    LOG_ERROR("null pointer!");
}

void
rdgl_graph_wadd_edge(rdgl_graph_t * g, rdgl_edge_t * e)
{
  if (g && g->graph && e && e->edge)
    g->graph->add_edge(*(e->edge));
  else
    LOG_ERROR("null pointer!");
}

VALUE
rdgl_graph_wset_edge_id(rdgl_graph_t * g, rdgl_edge_t * e)
{
  VALUE retval = Qnil;
  if (g && g->graph && e && e->edge)
    retval = g->graph->set_edge_id(*(e->edge));
  else
    LOG_ERROR("null pointer!");
  return retval;
}

VALUE
rdgl_graph_wremove_edge(rdgl_graph_t * g, rdgl_edge_t * e)
{
  RDGL::Edge * se = NULL;
  if (g && g->graph && e && e->edge)
    se = g->graph->remove_edge(*(e->edge));
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_wremove_edge");
  return se ? se->value : Qnil;
}

VALUE
rdgl_graph_wrandom_edge(rdgl_graph_t * g)
{
  RDGL::Edge * se = NULL;
  if (g && g->graph)
    se = g->graph->random_edge();
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_wrandom_edge");
  return se ? se->value : Qnil;
}

void
rdgl_graph_wadd_vertex(rdgl_graph_t * g, rdgl_vertex_t * v)
{
  if (g && g->graph && v && v->vertex)
    g->graph->add_vertex(*(v->vertex));
  else
    LOG_ERROR("null pointer!");
}

VALUE
rdgl_graph_wremove_vertex(rdgl_graph_t * g, rdgl_vertex_t * v)
{
  RDGL::Vertex * sv = NULL;
  if (g && g->graph && v && v->vertex)
    sv = g->graph->remove_vertex(*(v->vertex));
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_wremove_vertex");
  return sv ? sv->value : Qnil;
}

VALUE
rdgl_graph_wrandom_vertex(rdgl_graph_t * g)
{
  RDGL::Vertex * sv = NULL;
  if (g && g->graph)
    sv = g->graph->random_vertex();
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_wrandom_vertex");
  return sv ? sv->value : Qnil;
}

void
rdgl_graph_wreset(rdgl_graph_t * g, rdgl_vertex_t * s)
{
  if (g && g->graph && s && s->vertex)
    g->graph->reset(*(s->vertex));
  else
    LOG_ERROR("null pointer!");
}

// FIXME: there really needs to be a way to return an error condition
int
rdgl_graph_wget_num_vertices(rdgl_graph_t * g)
{
  if (g && g->graph)
    return g->graph->num_vertices();
  LOG_ERROR("null pointer!");
  return 0;
}

// FIXME: there really needs to be a way to return an error condition
int
rdgl_graph_wget_num_edges(rdgl_graph_t * g)
{
  if (g && g->graph)
    return g->graph->num_edges();
  LOG_ERROR("null pointer!");
  return 0;
}

int
rdgl_graph_wrun_dijkstra(rdgl_graph_t * g, rdgl_vertex_t * s)
{
  if (g && g->graph && s && s->vertex)
    return g->graph->run_dijkstra(*(s->vertex)) ? 1 : 0;
  LOG_ERROR("null pointer!");
  return 0;
}

int
rdgl_graph_wrun_bellman_ford(rdgl_graph_t * g, rdgl_vertex_t * s)
{
  if (g && g->graph && s && s->vertex)
    return g->graph->run_bellman_ford(*(s->vertex)) ? 1 : 0;
  LOG_ERROR("null pointer!");
  return 0;
}

int
rdgl_graph_wrun_yen(rdgl_graph_t * g, rdgl_vertex_t * s, rdgl_vertex_t * e)
{
  if (g && g->graph && s && s->vertex && e && e->vertex)
    return g->graph->run_yen(*(s->vertex), *(e->vertex)) ? 1 : 0;
  LOG_ERROR("null pointer!");
  return 0;
}

VALUE
rdgl_graph_wrun_floyd_warshall(rdgl_graph_t * g)
{
  if (g && g->graph)
    return g->graph->run_ruby_floyd_warshall();
  LOG_ERROR("null pointer!");
  return 0;
}

VALUE
rdgl_graph_wextract_path(rdgl_graph_t * g, rdgl_vertex_t * e)
{
  return g->graph->extract_ruby_path(*(e->vertex));
}

// print a graph
VALUE
rdgl_graph_wto_s(rdgl_graph_t * g)
{
  std::stringstream s;
  if (g && g->graph)
  {
    s << *(g->graph);
    return rb_str_new2(s.str().c_str());
  }
  LOG_ERROR("null pointer!");
  return Qnil;
}

} /* extern "C" { ... } */

void
rdgl_graph_wset_value(rdgl_graph_t * g, VALUE v)
{
  g->graph->value = v;
}

