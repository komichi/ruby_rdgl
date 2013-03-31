#include "ruby.h"

#undef DEBUG
//#define DEBUG

#ifdef __cplusplus
extern "C" {
#endif

#include "graph_cwrap.h"
#include "graph_general.h"

void Init_rdgl();

static VALUE
rdgl_str_cat(VALUE obj, VALUE cat)
{
  // basically this is just: cat += obj.to_s 
  ID to_s_id = rb_intern("to_s");
  VALUE new_str = rb_funcall(obj, to_s_id, 0);
  return rb_str_concat(cat, new_str);
}

// this is to handle 64 bit id's ... allowing them to be init'd by Fixnum's
static double
rdgl_value_to_double(VALUE obj)
{
  double v = 0.0;
  // take a Fixnum immediate value and grab its value
  if (FIXNUM_P(obj))
    return NUM2DBL(obj);
  // if float
  return RFLOAT_VALUE(obj);
  // else call to_f
  fprintf(stderr, "badness class is %d tfl is %d!\n", CLASS_OF(obj), T_FLOAT);
  return v; // FIXME: raise error
}

/******************************************************************************/
/* AllPairsMap ****************************************************************/
/******************************************************************************/

static VALUE
rdgl_apm_init(VALUE self)
{
  LOG_DEBUG("");
  /*rdgl_all_pairs_map_t * m = NULL;
  Data_Get_Struct(self, rdgl_all_pairs_map_t, m);
  if (! m || ! m->map)
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_apm_init");*/
  return self;
}

static VALUE
rdgl_apm_new(VALUE class)
{
  VALUE rm;
  rdgl_all_pairs_map_t * m = NULL;
  LOG_DEBUG("")
  m = rdgl_apm_wnew();
  rm = Data_Wrap_Struct(class, 0, rdgl_apm_wfree, m);
  return rm;
}

static VALUE
rdgl_apm_extract_path(VALUE self, VALUE startv, VALUE endv)
{
  rdgl_all_pairs_map_t * m = NULL;
  rdgl_vertex_t * s = NULL, * t = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_all_pairs_map_t, m);
  Data_Get_Struct(startv, rdgl_vertex_t, s);
  Data_Get_Struct(endv, rdgl_vertex_t, t);
  if (m && m->map && s && s->vertex && t && t->vertex)
    return rdgl_apm_wextract_ruby_path(m, s, t);
  else 
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_apm_extract_path");
}

static VALUE
rdgl_apm_extract_all_paths(VALUE self)
{
  rdgl_all_pairs_map_t * m = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_all_pairs_map_t, m);
  if (m && m->map)
    return rdgl_apm_wextract_ruby_all_paths(m);
  else 
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_apm_extract_all_paths");
}

static VALUE
rdgl_apm_to_s(VALUE self)
{
  rdgl_all_pairs_map_t * m = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_all_pairs_map_t, m);
  if (m && m->map)
    return rdgl_apm_wto_s(m);
  else 
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_apm_to_s");
}

/******************************************************************************/
/* Path ***********************************************************************/
/******************************************************************************/

static VALUE
rdgl_path_add_edge(VALUE self, VALUE edge)
{
  rb_ary_push(self, edge);
  return self;
}

static VALUE
rdgl_path_to_s(VALUE self)
{
  VALUE str = rb_str_new2("Path:\n");
  rb_iterate(rb_each, self, rdgl_str_cat, str);
  //rb_str_cat(str, "\n", 1);
  return str;
}

/******************************************************************************/
/* PathSet ********************************************************************/
/******************************************************************************/

static VALUE
rdgl_pathset_add_path(VALUE self, VALUE path)
{
  rb_ary_push(self, path);
  return self;
}

static VALUE
rdgl_pathset_to_s(VALUE self)
{
  VALUE str = rb_str_new2("Pathset(\n");
  rb_iterate(rb_each, self, rdgl_str_cat, str);
  rb_str_cat(str, ")\n", 2);
  return str;
}

/******************************************************************************/
/* Vertex *********************************************************************/
/******************************************************************************/

// FIXME: check types of args using rb_obj_is_kind_of(...)
static VALUE
rdgl_vertex_init(VALUE self, VALUE id, VALUE distance)
{
  rdgl_vertex_t * v = NULL;
  rdgl_vertex_distance_t d;
  LOG_DEBUG("");
  //Check_Type(distance, T_FLOAT);
  //d = RFLOAT(distance)->value;
  d = rdgl_value_to_double(distance);
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
  {
    rdgl_vertex_wset_id(v, NUM2ULL(id));
    rdgl_vertex_wset_distance(v, d);
  }
  else
  {
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_vertex_init");
  }
  return self;
}

static VALUE
rdgl_vertex_new(VALUE class, VALUE id, VALUE distance)
{
  VALUE rv;
  rdgl_vertex_t * v = NULL;
  rdgl_vertex_distance_t d = RDGL_GRAPH_INFINITY;
  LOG_DEBUG("")
  d = rdgl_value_to_double(distance);
  //Check_Type(distance, T_FLOAT);
  //d = RFLOAT(distance)->value;
  v = rdgl_vertex_wnew(NUM2ULL(id), d);
  rv = Data_Wrap_Struct(class, 0, rdgl_vertex_wfree, v);
  rdgl_vertex_wset_value(v, rv);
  return rv;
}

static VALUE
rdgl_vertex_get_id(VALUE self)
{
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
    return ULL2NUM(rdgl_vertex_wget_id(v));
  else 
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_vertex_get_id");
}

static VALUE
rdgl_vertex_set_id(VALUE self, VALUE id)
{
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
    rdgl_vertex_wset_id(v, NUM2ULL(id));
  else 
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_vertex_set_id");
  return id;
}

static VALUE
rdgl_vertex_get_distance(VALUE self)
{
  rdgl_vertex_t * v = NULL;
  Data_Get_Struct(self, rdgl_vertex_t, v);
  LOG_DEBUG("")
  if (v && v->vertex)
    return VERTEX_DISTANCE_TO_VALUE(rdgl_vertex_wget_distance(v));
  rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_get_distance");
}

static VALUE
rdgl_vertex_set_distance(VALUE self, VALUE distance)
{
  rdgl_vertex_t * v = NULL;
  rdgl_vertex_distance_t d;
  LOG_DEBUG("");
  //Check_Type(distance, T_FLOAT);
  //d = RFLOAT(distance)->value;
  d = rdgl_value_to_double(distance);
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
    rdgl_vertex_wset_distance(v, d);
  else
    rb_raise(rb_eStandardError, "null pointer or insufficient data in rdgl_vertex_set_distance");
  return distance;
}

static VALUE
rdgl_vertex_add_out_edge(VALUE self, VALUE edge)
{
  rdgl_vertex_t * v = NULL;
  rdgl_edge_t * e = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, v);
  Data_Get_Struct(edge, rdgl_edge_t, e);
  if (v && v->vertex && e && e->edge)
    rdgl_vertex_wadd_out_edge(v, e);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_add_out_edge");
  return edge;
}

static VALUE
rdgl_vertex_set_back_edge(VALUE self, VALUE edge)
{
  rdgl_vertex_t * v = NULL;
  rdgl_edge_t * e = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, v);
  Data_Get_Struct(edge, rdgl_edge_t, e);
  if (v && v->vertex && e)
    rdgl_vertex_wset_back_edge(v, e);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_set_back_edge");
  return edge;
}

static VALUE
rdgl_vertex_get_back_edge(VALUE self)
{
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
    return rdgl_vertex_wget_back_edge(v);
  rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_get_back_edge");
}

static VALUE
rdgl_vertex_to_s(VALUE self)
{
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
    return rdgl_vertex_wto_s(v);
  rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_to_s");
}

static VALUE
rdgl_vertex_inspect(VALUE self)
{
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, v);
  if (v && v->vertex)
    return rdgl_vertex_winspect(v);
  rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_inspect");
}

static VALUE
rdgl_vertex_equality(VALUE self, VALUE vertex)
{
  int equal = -1;
  rdgl_vertex_t * a = NULL,
                * b = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_vertex_t, a);
  Data_Get_Struct(vertex, rdgl_vertex_t, b);
  if (a && a->vertex && b && b->vertex)
    equal = rdgl_vertex_wequality(a, b);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_vertex_equality");
  if (equal != -1) return (equal==1) ? Qtrue : Qfalse;
  return Qnil;
}

static VALUE
rdgl_vertex_inequality(VALUE self, VALUE vertex)
{
  VALUE ret = rdgl_vertex_equality(self, vertex);
  if (ret == Qtrue) return Qfalse;
  if (ret == Qfalse) return Qtrue;
  return Qnil;
}

/******************************************************************************/
/* Edge ***********************************************************************/
/******************************************************************************/

static VALUE
rdgl_edge_init(int argc, VALUE *argv, VALUE self)
{
  rdgl_edge_t * e = NULL;
  rdgl_vertex_t * s = NULL, * t = NULL;
  rdgl_edge_weight_t w = 1.0;
  rdgl_edge_id_t edge_id = 0;
  rdgl_graph_t * graph = NULL;
  LOG_DEBUG("got %d args", argc)
  if (argc > 0) Data_Get_Struct(argv[0], rdgl_vertex_t, s);
  if (argc > 1) Data_Get_Struct(argv[1], rdgl_vertex_t, t);
  if (argc > 2) w = rdgl_value_to_double(argv[2]);
  if (argc > 3) edge_id = (argv[3] == Qnil) ? 0 :
                          (rdgl_edge_id_t) NUM2LONG(argv[3]);
  if (argc > 4) Data_Get_Struct(argv[4], rdgl_graph_t, graph);
  Data_Get_Struct(self, rdgl_edge_t, e);
  if (e && e->edge && s && s->vertex && t && t->vertex)
  {
    rdgl_edge_wset_start_vertex(e, s);
    rdgl_edge_wset_end_vertex(e, t);
    rdgl_edge_wset_weight(e, w);
    rdgl_edge_wset_edge_id(e, edge_id, graph);
  }
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_init");
  return self;
}

static VALUE
rdgl_edge_new(int argc, VALUE *argv, VALUE class)
{
  VALUE re;
  rdgl_edge_t * e = NULL;
  rdgl_vertex_t * s = NULL, * t = NULL;
  rdgl_graph_t * graph = NULL;
  rdgl_edge_weight_t w = 1.0;
  rdgl_edge_id_t edge_id = 0;
  LOG_DEBUG("got %d args", argc)
  if (argc > 0) Data_Get_Struct(argv[0], rdgl_vertex_t, s);
  if (argc > 1) Data_Get_Struct(argv[1], rdgl_vertex_t, t);
  if (argc > 2) w = rdgl_value_to_double(argv[2]);
  if (argc > 3) edge_id = (argv[3] == Qnil) ? 0 :
                          (rdgl_edge_id_t) NUM2LONG(argv[3]);
  if (argc > 4) Data_Get_Struct(argv[4], rdgl_graph_t, graph);
  e = rdgl_edge_wnew(s, t, w, edge_id, graph);
  if (! e) rb_raise(rb_eStandardError, "malloc failed in rdgl_edge_new");
  re = Data_Wrap_Struct(class, 0, rdgl_edge_wfree, e);
  rdgl_edge_wset_value(e, re);
  return re;
}

static VALUE
rdgl_edge_relax(VALUE self)
{
  rdgl_edge_t * e = NULL;
  Data_Get_Struct(self, rdgl_edge_t, e);
  LOG_DEBUG("")
  if (e && e->edge)
    rdgl_edge_wrelax(e);
  else
    rb_raise(rb_eStandardError, "malloc failed in rdgl_edge_relax");
  return Qnil;
}

static VALUE
rdgl_edge_reset(VALUE self)
{
  rdgl_edge_t * e = NULL;
  Data_Get_Struct(self, rdgl_edge_t, e);
  LOG_DEBUG("")
  if (e && e->edge)
    rdgl_edge_wreset(e);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_reset");
  return Qnil;
}

static VALUE
rdgl_edge_get_weight(VALUE self)
{
  rdgl_edge_t * e = NULL;
  Data_Get_Struct(self, rdgl_edge_t, e);
  LOG_DEBUG("")
  if (e && e->edge)
    return rb_float_new(rdgl_edge_wget_weight(e));
  rb_raise(rb_eStandardError, "null pointer in rdgl_edge_get_weight");
}

static VALUE
rdgl_edge_set_weight(VALUE self, VALUE weight)
{
  rdgl_edge_t * e = NULL;
  rdgl_edge_weight_t w;
  LOG_DEBUG("")
  //Check_Type(weight, T_FLOAT);
  Data_Get_Struct(self, rdgl_edge_t, e);
  //w = RFLOAT(weight)->value;
  w = rdgl_value_to_double(weight);
  if (e && e->edge)
    rdgl_edge_wset_weight(e, w);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_set_weight");
  return weight;
}

static VALUE
rdgl_edge_get_edge_id(VALUE self)
{
  rdgl_edge_t * e = NULL;
  Data_Get_Struct(self, rdgl_edge_t, e);
  LOG_DEBUG("")
  if (e && e->edge)
    return ULL2NUM(rdgl_edge_wget_edge_id(e));
  rb_raise(rb_eStandardError, "null pointer in rdgl_edge_get_edge_id");
}

static VALUE
rdgl_edge_set_edge_id(VALUE self, VALUE edge_id)
{
  rdgl_edge_t * e = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_edge_t, e);
  if (e && e->edge)
    rdgl_edge_wset_edge_id(e, (rdgl_edge_id_t) NUM2LONG(edge_id), NULL);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_set_edge_id");
  return edge_id;
}

static VALUE
rdgl_edge_set_start_vertex(VALUE self, VALUE start)
{
  rdgl_edge_t * e = NULL;
  rdgl_vertex_t * s = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_edge_t, e);
  Data_Get_Struct(start, rdgl_vertex_t, s);
  if (e && e->edge && s && s->vertex)
    rdgl_edge_wset_start_vertex(e, s);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_set_start_vertex");
  return start;
}

static VALUE
rdgl_edge_set_end_vertex(VALUE self, VALUE end)
{
  rdgl_edge_t * e = NULL;
  rdgl_vertex_t * t = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_edge_t, e);
  Data_Get_Struct(end, rdgl_vertex_t, t);
  if (e && e->edge && t && t->vertex)
    rdgl_edge_wset_end_vertex(e, t);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_set_end_vertex");
  return end;
}

static VALUE
rdgl_edge_to_s(VALUE self)
{
  rdgl_edge_t * e = NULL;
  const char * s = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_edge_t, e);
  if (e && e->edge)
    return rdgl_edge_wto_s(e);
  rb_raise(rb_eStandardError, "null pointer in rdgl_edge_to_s");
}

static VALUE
rdgl_edge_equality(VALUE self, VALUE edge)
{
  int equal = -1;
  rdgl_edge_t * a = NULL,
              * b = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_edge_t, a);
  Data_Get_Struct(edge, rdgl_edge_t, b);
  if (a && a->edge && b && b->edge)
    equal = rdgl_edge_wequality(a, b);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_edge_equality");
  if (equal != -1) return (equal==1) ? Qtrue : Qfalse;
  return Qnil;
}

static VALUE
rdgl_edge_inequality(VALUE self, VALUE edge)
{
  VALUE ret = rdgl_edge_inequality(self, edge);
  if (ret == Qtrue) return Qfalse;
  if (ret == Qfalse) return Qtrue;
  return Qnil;
}

/******************************************************************************/
/* Graph **********************************************************************/
/******************************************************************************/

static VALUE
rdgl_graph_init(VALUE self)
{
  LOG_DEBUG("");
  return self;
}

static void
rdgl_graph_mark(rdgl_graph_t * g)
{
  //rdgl_graph_t * g = NULL;
  LOG_DEBUG("")
  //Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    rdgl_graph_wmark(g);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_mark");
}

static VALUE
rdgl_graph_new(VALUE class)
{
  VALUE rg;
  rdgl_graph_t * g = NULL;
  LOG_DEBUG("")
  g = rdgl_graph_wnew();
  if (! g)
    rb_raise(rb_eStandardError, "malloc failed in rdgl_graph_new");
  rg = Data_Wrap_Struct(class, rdgl_graph_mark, rdgl_graph_wfree, g);
  rdgl_graph_wset_value(g, rg);
  return rg;
}

static VALUE
rdgl_graph_add_edge(VALUE self, VALUE edge)
{
  rdgl_graph_t * g = NULL;
  rdgl_edge_t * e = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(edge, rdgl_edge_t, e);
  if (g && g->graph && e && e->edge)
    rdgl_graph_wadd_edge(g, e);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_add_edge");
  return edge;
}

static VALUE
rdgl_graph_set_edge_id(VALUE self, VALUE edge)
{
  VALUE retval = Qnil;
  rdgl_graph_t * g = NULL;
  rdgl_edge_t * e = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(edge, rdgl_edge_t, e);
  if (g && g->graph && e && e->edge)
    retval = rdgl_graph_wset_edge_id(g, e);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_set_edge_id");
  return retval;
}

static VALUE
rdgl_graph_remove_edge(VALUE self, VALUE edge)
{
  rdgl_graph_t * g = NULL;
  rdgl_edge_t * e = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(edge, rdgl_edge_t, e);
  if (g && g->graph && e && e->edge)
    return rdgl_graph_wremove_edge(g, e);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_remove_edge");
}

static VALUE
rdgl_graph_random_edge(VALUE self)
{
  rdgl_graph_t * g = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    return rdgl_graph_wrandom_edge(g);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_random_edge");
}

static VALUE
rdgl_graph_add_vertex(VALUE self, VALUE vertex)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(vertex, rdgl_vertex_t, v);
  if (g && g->graph && v && v->vertex)
    rdgl_graph_wadd_vertex(g, v);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_add_vertex");
  return vertex;
}

static VALUE
rdgl_graph_remove_vertex(VALUE self, VALUE vertex)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * v = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(vertex, rdgl_vertex_t, v);
  if (g && g->graph && v && v->vertex)
    return rdgl_graph_wremove_vertex(g, v);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_remove_vertex");
}

static VALUE
rdgl_graph_random_vertex(VALUE self)
{
  rdgl_graph_t * g = NULL;
  LOG_DEBUG("");
  Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    return rdgl_graph_wrandom_vertex(g);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_random_vertex");
}

static VALUE
rdgl_graph_reset(VALUE self, VALUE start)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * s = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(start, rdgl_vertex_t, s);
  if (g && g->graph && s && s->vertex)
    rdgl_graph_wreset(g, s);
  else
    rb_raise(rb_eStandardError, "null pointer in rdgl_graph_reset");
  return Qtrue;
}

static VALUE
rdgl_graph_get_num_vertices(VALUE self)
{
  rdgl_graph_t * g = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    return INT2NUM(rdgl_graph_wget_num_vertices(g));
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_get_num_vertices");
}

static VALUE
rdgl_graph_get_num_edges(VALUE self)
{
  rdgl_graph_t * g = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    return INT2NUM(rdgl_graph_wget_num_edges(g));
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_get_num_edges");
}

static VALUE
rdgl_graph_run_dijkstra(VALUE self, VALUE start)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * s = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(start, rdgl_vertex_t, s);
  if (g && g->graph && s && s->vertex)
    return rdgl_graph_wrun_dijkstra(g, s) ? Qtrue : Qnil;
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_run_dijkstra");
}

static VALUE
rdgl_graph_run_bellman_ford(VALUE self, VALUE start)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * s = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(start, rdgl_vertex_t, s);
  if (g && g->graph && s && s->vertex)
    return rdgl_graph_wrun_bellman_ford(g, s) ? Qtrue : Qnil;
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_run_bellman_ford");
}

static VALUE
rdgl_graph_run_yen(VALUE self, VALUE start, VALUE end)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * s = NULL, * t = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(start, rdgl_vertex_t, s);
  Data_Get_Struct(end, rdgl_vertex_t, t);
  if (g && g->graph && s && s->vertex && t && t->vertex)
    return rdgl_graph_wrun_yen(g, s, t) ? Qtrue : Qnil;
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_run_yen");
}

static VALUE
rdgl_graph_run_floyd_warshall(VALUE self)
{
  rdgl_graph_t * g = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    return rdgl_graph_wrun_floyd_warshall(g);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_run_floyd_warshall");
}

static VALUE
rdgl_graph_extract_path(VALUE self, VALUE end)
{
  rdgl_graph_t * g = NULL;
  rdgl_vertex_t * e = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  Data_Get_Struct(end, rdgl_vertex_t, e);
  if (g && g->graph && e && e->vertex)
    return rdgl_graph_wextract_path(g, e);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_extract_path");
}

static VALUE
rdgl_graph_to_s(VALUE self)
{
  rdgl_graph_t * g = NULL;
  const char * s = NULL;
  LOG_DEBUG("")
  Data_Get_Struct(self, rdgl_graph_t, g);
  if (g && g->graph)
    return rdgl_graph_wto_s(g);
  rb_raise(rb_eStandardError, "null pointer in rdgl_graph_to_s");
}

/******************************************************************************/
/* Interpreter Stuff **********************************************************/
/******************************************************************************/

void
Init_rdgl()
{
  LOG_DEBUG("defining modules, classes and constants");
  VALUE mRdgl            = rb_define_module("Rdgl");
  VALUE cRdglVertex      = rb_define_class_under(mRdgl, "Vertex", rb_cObject);
  VALUE cRdglGraph       = rb_define_class_under(mRdgl, "Graph", rb_cObject);
  VALUE cRdglEdge        = rb_define_class_under(mRdgl, "Edge", rb_cObject);
  VALUE cRdglPath        = rb_define_class_under(mRdgl, "Path", rb_cArray);
  VALUE cRdglPathSet     = rb_define_class_under(mRdgl, "PathSet", rb_cArray);
  VALUE cRdglAllPairsMap = rb_define_class_under(mRdgl, "AllPairsMap", rb_cObject);
  VALUE sgInf            = rb_float_new(RDGL_GRAPH_INFINITY);
  rb_define_const(mRdgl, "INFINITY", sgInf);

/* AllPairsMap ****************************************************************/
  rb_define_singleton_method(cRdglAllPairsMap, "new", rdgl_apm_new, 0);
  // FIXME: I do not understand the difference between using a new "singleton"
  //        and explicitly defining a (single arg) alloc function as here, but
  //        this does not work (new is never invoked on rb_clas..instance..reate
  rb_define_alloc_func(cRdglAllPairsMap, rdgl_apm_new);
  rb_define_method(cRdglAllPairsMap, "initialize",        rdgl_apm_init, 0);
  rb_define_method(cRdglAllPairsMap, "extract_path",      rdgl_apm_extract_path, 2);
  rb_define_method(cRdglAllPairsMap, "extract_all_paths", rdgl_apm_extract_all_paths, 0);
  rb_define_method(cRdglAllPairsMap, "to_s",              rdgl_apm_to_s, 0);

/* Path ***********************************************************************/
  rb_define_method(cRdglPath, "add_edge", rdgl_path_add_edge, 1);
  rb_define_method(cRdglPath, "to_s",     rdgl_path_to_s, 0);

/* PathSet ********************************************************************/
  rb_define_method(cRdglPathSet, "add_path", rdgl_pathset_add_path, 1);
  rb_define_method(cRdglPathSet, "to_s",     rdgl_pathset_to_s, 0);

/* Vertex *********************************************************************/
  rb_define_singleton_method(cRdglVertex, "new", rdgl_vertex_new, 2);
  rb_define_method(cRdglVertex, "initialize",    rdgl_vertex_init, 2);
  rb_define_method(cRdglVertex, "vertex_id",     rdgl_vertex_get_id, 0);
  rb_define_method(cRdglVertex, "vertex_id=",    rdgl_vertex_set_id, 1);
  rb_define_method(cRdglVertex, "distance",      rdgl_vertex_get_distance, 0);
  rb_define_method(cRdglVertex, "distance=",     rdgl_vertex_set_distance, 1);
  rb_define_method(cRdglVertex, "back_edge",     rdgl_vertex_get_back_edge,0);
  rb_define_method(cRdglVertex, "back_edge=",    rdgl_vertex_set_back_edge, 1);
  rb_define_method(cRdglVertex, "add_out_edge",  rdgl_vertex_add_out_edge, 1);
  rb_define_method(cRdglVertex, "to_s",          rdgl_vertex_to_s, 0);
  rb_define_method(cRdglVertex, "inspect",       rdgl_vertex_inspect, 0);
  rb_define_method(cRdglVertex, "==",            rdgl_vertex_equality, 1);
  rb_define_method(cRdglVertex, "!=",            rdgl_vertex_inequality, 1);

/* Edge *******************************************************************/
  rb_define_singleton_method(cRdglEdge, "new", rdgl_edge_new, -1);
  rb_define_method(cRdglEdge, "initialize",    rdgl_edge_init, -1);
  rb_define_method(cRdglEdge, "relax",         rdgl_edge_relax, 0);
  rb_define_method(cRdglEdge, "reset",         rdgl_edge_reset, 0);
  rb_define_method(cRdglEdge, "weight",        rdgl_edge_get_weight, 0);
  rb_define_method(cRdglEdge, "weight=",       rdgl_edge_set_weight, 1);
  rb_define_method(cRdglEdge, "edge_id",       rdgl_edge_get_edge_id, 0);
  rb_define_method(cRdglEdge, "edge_id=",      rdgl_edge_set_edge_id, 1);
  rb_define_method(cRdglEdge, "start_vertex=", rdgl_edge_set_start_vertex, 1);
  rb_define_method(cRdglEdge, "end_vertex=",   rdgl_edge_set_end_vertex, 1);
  rb_define_method(cRdglEdge, "to_s",          rdgl_edge_to_s, 0);
  rb_define_method(cRdglEdge, "==",            rdgl_edge_equality, 1);
  rb_define_method(cRdglEdge, "!=",            rdgl_edge_inequality, 1);

/* Graph **********************************************************************/
  rb_define_singleton_method(cRdglGraph, "new",      rdgl_graph_new, 0);
  rb_define_method(cRdglGraph, "initialize",         rdgl_graph_init, 0);
  rb_define_method(cRdglGraph, "add_edge",           rdgl_graph_add_edge, 1);
  rb_define_method(cRdglGraph, "set_edge_id",        rdgl_graph_set_edge_id, 1);
  rb_define_method(cRdglGraph, "remove_edge",        rdgl_graph_remove_edge, 1);
  rb_define_method(cRdglGraph, "random_edge",        rdgl_graph_random_edge, 0);
  rb_define_method(cRdglGraph, "add_vertex",         rdgl_graph_add_vertex, 1);
  rb_define_method(cRdglGraph, "remove_vertex",      rdgl_graph_remove_vertex, 1);
  rb_define_method(cRdglGraph, "random_vertex",      rdgl_graph_random_vertex, 0);
  rb_define_method(cRdglGraph, "reset",              rdgl_graph_reset, 1);
  rb_define_method(cRdglGraph, "num_edges",          rdgl_graph_get_num_edges, 0);
  rb_define_method(cRdglGraph, "size",               rdgl_graph_get_num_edges, 0);
  rb_define_method(cRdglGraph, "num_vertices",       rdgl_graph_get_num_vertices, 0);
  rb_define_method(cRdglGraph, "order",              rdgl_graph_get_num_vertices, 0);
  rb_define_method(cRdglGraph, "run_dijkstra",       rdgl_graph_run_dijkstra, 1);
  rb_define_method(cRdglGraph, "run_bellman_ford",   rdgl_graph_run_bellman_ford, 1);
  rb_define_method(cRdglGraph, "run_floyd_warshall", rdgl_graph_run_floyd_warshall, 0);
  rb_define_method(cRdglGraph, "run_yen",            rdgl_graph_run_yen, 2);
  rb_define_method(cRdglGraph, "extract_path",       rdgl_graph_extract_path, 1);
  rb_define_method(cRdglGraph, "to_s",               rdgl_graph_to_s, 0);
}

#ifdef __cplusplus
} /* extern "C" */
#endif

