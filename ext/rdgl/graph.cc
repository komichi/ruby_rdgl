#include "graph.h"

#include <vector>

namespace RDGL {

#ifdef RUBY_H
static VALUE
spath_to_rpath(Path & p) // Path & p => VALUE (Ruby Path)
{
  Path::iterator i;
  VALUE mRdgl = rb_define_module("Rdgl");
  VALUE cPath = rb_define_class_under(mRdgl, "Path", rb_cArray); // FIXME: isn't there a better way to get a reference?
  VALUE v = rb_class_new_instance(0, NULL, cPath);
  ID add_edge_id = rb_intern("add_edge");
  for (i = p.begin(); i != p.end(); i++)
    rb_funcall(v, add_edge_id, 1, (*i)->value);
  return v;
}

static VALUE
spaths_to_rpaths(PathSet & p) // PathSet & p => VALUE (Ruby PathSet)
{
  PathSet::iterator i;
  VALUE mRdgl = rb_define_module("Rdgl");
  VALUE cPathSet = rb_define_class_under(mRdgl, "PathSet", rb_cObject);
  VALUE v = rb_class_new_instance(0, NULL, cPathSet);
  ID add_path_id = rb_intern("add_path");
  for (i = p.begin(); i != p.end(); i++)
    rb_funcall(v, add_path_id, 1, spath_to_rpath(*(*i)));
  return v;
}
#endif

/******************************************************************************/
/* Vertex *********************************************************************/
/******************************************************************************/

static int get_random(int low, int high)
{
  static bool initialized = false;
  if (! initialized)
  {
    srand((unsigned)time(0)); 
    initialized = true;
  }
  int range = high - low;
  double r = (double) rand() / (double) RAND_MAX;
  return int((r * (double) range) + (double) low);
}

// null constructor
Vertex::Vertex(void)
{
  out_edges = new EdgeSet;
  reset();
}

Vertex::Vertex(rdgl_vertex_id_t new_id, rdgl_edge_weight_t new_distance=0.0)
{
  out_edges = new EdgeSet;
  reset();
  id = new_id;
  distance = new_distance;
}

// copy constructor
Vertex::Vertex(const Vertex & v)
{
  out_edges = new EdgeSet;
  reset();
}

// destructor
Vertex::~Vertex(void)
{
  if (out_edges)
  {
    out_edges->clear();
    delete out_edges;
    out_edges = NULL;
  }
}

// comparison operators
bool
Vertex::operator<(const Vertex & b) const
{ 
  return (distance < b.distance) || ((distance == b.distance) && (id < b.id));
}

bool
Vertex::operator>(const Vertex & b) const
{
  return (distance > b.distance) || ((distance == b.distance) && (id > b.id));
}

bool
Vertex::operator==(const Vertex & b) const
{
  return (id == b.id); // && (distance == b.distance);
}

bool
Vertex::operator!=(const Vertex & b) const
{
  return (id != b.id); // && (distance == b.distance);
}

bool
vertex_id_lt_cmp(const Vertex * a, const Vertex * b)
{
  return a->id < b->id; // compare by id only
}

bool
Vertex::operator()(const Vertex * a, Vertex * b) const
{
  return vertex_id_lt_cmp(a, b);
}

// resets member data
void
Vertex::reset(void)
{
  id = -1;
  distance = MAX_DISTANCE;
  //successor = predecessor = NULL;
  back_edge = forward_edge = NULL;
  if (out_edges)
    out_edges->clear(); // delete all out_edge entries
#ifdef RUBY_H
  value = (VALUE) 0;
#endif
}

// adds out edges
void
Vertex::add_out_edge(Edge & e)
{
  if (! out_edges)
  {
    std::cerr << "NULL pointer in Vertex::add_out_edge" << std::endl;
    return;
  }
  out_edges->insert(e);
}

std::ostream &
operator<<(std::ostream & os, Vertex & v)
{
#ifdef DEBUG
  os << "a="
     << &v << "; ";
#endif
  os << "id="
     << v.id
     << " (d=";
  if (v.distance == MAX_DISTANCE) os << "INF";
  else os << v.distance;
  os << "; n="
     << v.out_edges->size()
     << "; b=";
  if (v.back_edge) os << (int) v.back_edge->edge_id;
  else os << "NIL";
  os << "; f=";
  if (v.forward_edge) os << "<" << v.forward_edge->start_vertex->id
                         << "," << (int) v.forward_edge->edge_id
                         << "," << v.forward_edge->end_vertex->id << ">";
  else os << "NIL";
  os << ") ";
  return os;
}

/******************************************************************************/
/* VertexSet ******************************************************************/
/******************************************************************************/

Vertex *
VertexSet::find(Vertex & v)
{
  VertexSet::iterator vi;
  for (vi = this->begin(); vi != this->end(); vi++)
    if (**vi == v) return *vi;
  return NULL;
}

Vertex *
VertexSet::random(void)
{
  int j=0, max = get_random(0, this->size()-1);
  VertexSet::iterator vi;
  for (j=0, vi = this->begin(); (vi != this->end()) && (j < max); vi++, j++) {;}
  return *vi;
}

/******************************************************************************/
/* EdgeSet ********************************************************************/
/******************************************************************************/

Edge *
EdgeSet::find(Edge & e)
{
  EdgeSet::iterator ei;
  for (ei = this->begin(); ei != this->end(); ei++)
    if (**ei == e)
      return *ei;
    //if (! rdgl_edge_lt_cmp(*ei, &e) && ! rdgl_edge_lt_cmp(&e, *ei))
    //if (! rdgl_edge_lt_cmp(*ei, &e) && ! rdgl_edge_lt_cmp(&e, *ei))
      //return *ei;
  return NULL;
}

Edge *
EdgeSet::random(void)
{
  int j=0, max = get_random(0, this->size()-1);
  EdgeSet::iterator ei;
  for (j=0, ei = this->begin(); (ei != this->end()) && (j < max); ei++, j++) {;}
  return *ei;
}

/******************************************************************************/
/* VertexList *****************************************************************/
/******************************************************************************/

Vertex *
VertexList::remove_smallest(void)
{
  Vertex * v = NULL;
  if (this->size() == 0) return NULL;
  VertexList::iterator i, smallest;
  i = smallest = this->begin();
  for (++i; i != this->end(); i++)
    if (*i && (**i < **smallest))
      smallest = i;
  v = *smallest;
  this->erase(smallest);
  return v;
}

void
VertexList::prepend(Vertex & v)
{
  std::vector<Vertex *>::insert(this->begin(), &v);
}

void
VertexList::insert(Vertex & v)
{
  std::vector<Vertex *>::push_back(&v);
}

void
VertexList::append(Vertex & v)
{
  std::vector<Vertex *>::push_back(&v);
}

std::ostream &
operator<<(std::ostream & os, VertexList & l)
{
  VertexList::iterator i;
  os << "vertex list: " << std::endl;
  for (i = l.begin(); i != l.end(); i++)
    os << **i << std::endl;
  return os;
}

/******************************************************************************/
/* VertexPriorityQueue ********************************************************/
/******************************************************************************/
/*
Vertex *
VertexPriorityQueue::push(Vertex & v)
{
  rdgl_VPQ_CLASS::push(&v);
  return &v;
}

Vertex *
VertexPriorityQueue::pop(Vertex & v)
{
  Vertex * min = rdgl_VPQ_CLASS::top();
  rdgl_VPQ_CLASS::pop();
  return min;
}

Vertex *
VertexPriorityQueue::min(Vertex & v)
{
  return rdgl_VPQ_CLASS::top();
}

// for whatever reason STL doesn't provide this; what follows isn't an
// replacement performance-wise (I think), but it is functional
void
VertexPriorityQueue::decrease_key(Vertex & v, rdgl_edge_weight_t d)
{
}

std::ostream &
operator<<(std::ostream & os, VertexPriorityQueue & q)
{
  os << q;
  return os;
}
*/

/******************************************************************************/
/* AllPairsMap ****************************************************************/
/******************************************************************************/

// start at 1 and use 0 to act like null
#define rdgl_ALL_PAIRS_MAP_START_ID 0
#define rdgl_ALL_PAIRS_MAP_NULL_ID (rdgl_ALL_PAIRS_MAP_START_ID-1)
#define rdgl_ALL_PAIRS_IS_NULL(x) (x==-1)
#define rdgl_ALL_PAIRS_NULL -1
#define rdgl_ALL_PAIRS_INFINITY DBL_MAX
#define rdgl_EXT2INT_VERTEX_ID(x) (x-rdgl_ALL_PAIRS_MAP_START_ID)
#define rdgl_INT2EXT_VERTEX_ID(x) (x+rdgl_ALL_PAIRS_MAP_START_ID)

#ifdef RUBY_H
VALUE
apm_to_rapm(AllPairsMap & map) // Graph & g => VALUE (AllPairsMap)
{
  rdgl_all_pairs_map_t * m = NULL;
  VALUE mRdgl = rb_define_module("Rdgl");
  //
  //fprintf(stderr, "in apm_to_rapm calling define graph\n");
  //VALUE cGraph = rb_define_class_under(mRdgl, "Graph", rb_cObject);
  //fprintf(stderr, "in apm_to_rapm calling new graph\n");
  //VALUE vg = rb_class_new_instance(0, NULL, cGraph);
  //
  VALUE cAllPairsMap = rb_define_class_under(mRdgl, "AllPairsMap", rb_cObject);
  //fprintf(stderr, "in apm_to_rapm calling new instance\n");
  VALUE v = rb_class_new_instance(0, NULL, cAllPairsMap);
  //fprintf(stderr, "in apm_to_rapm\n");
  Data_Get_Struct(v, rdgl_all_pairs_map_t, m);
  //fprintf(stderr, "in apm_to_rapm got all pairs map\n");
  m->map = &map;
  map.value = v;
  return v;
}
#endif

// FIXME: technically, the vertices vertex isn't required:
//        you could just use the map after reading the data out
AllPairsMap::AllPairsMap(Graph & g)
{
/*graph = NULL;
  distances = new Matrix<rdgl_edge_weight_t>(g.vertices->size(), rdgl_ALL_PAIRS_INFINITY);
  if (! distances)
  {
    distances = NULL;
    fprintf(stderr, "allocation in AllPairsMap failed\n");
    return;
  }
  predecessors = new Matrix<rdgl_all_pairs_map_id_t>(g.vertices->size(), rdgl_ALL_PAIRS_NULL);
  if (! predecessors)
  {
    delete distances;
    distances = NULL;
    predecessors = NULL;
    fprintf(stderr, "allocation in AllPairsMap failed\n");
    return;
  }
  vertices = new VertexList;
  if (! vertices)
  {
    delete distances;
    delete predecessors;
    distances = NULL;
    predecessors = NULL;
    vertices = NULL;
    fprintf(stderr, "allocation in AllPairsMap failed\n");
    return;
  }
  ptr2id = new std::map<Vertex *, rdgl_all_pairs_map_id_t>;
  if (! ptr2id)
  {
    delete distances;
    delete predecessors;
    delete vertices;
    distances = NULL;
    predecessors = NULL;
    vertices = NULL;
    ptr2id = NULL;
    fprintf(stderr, "allocation in AllPairsMap failed\n");
    return;
  }
  set_graph(g);
#ifdef RUBY_H
  apm_to_rapm(*this); // sets value
#endif
*/
}

void
AllPairsMap::set_vertex_id(Vertex & v, rdgl_all_pairs_map_id_t id)
{
/*  (*ptr2id)[&v] = id;*/
}

rdgl_all_pairs_map_id_t
AllPairsMap::get_id_from_vertex(Vertex & v)
{
/*return (*ptr2id)[&v];*/
  return rdgl_ALL_PAIRS_NULL;
}

Vertex *
AllPairsMap::get_vertex_from_id(rdgl_all_pairs_map_id_t i)
{
/*return (*vertices)[i];*/
  return NULL;
}

void
AllPairsMap::set_graph(Graph & g)
{
/*rdgl_all_pairs_map_id_t i, j;
  VertexSet::iterator vi;
  EdgeSet::iterator ei;
  if (graph) return;
  graph = &g;
  for (vi=g.vertices->begin(), i=0;
       vi != g.vertices->end();
       vi++, i++)
  {
    vertices->push_back(*vi);
    set_vertex_id(**vi, i);
  }
  for (ei=g.edges->begin(); ei != g.edges->end(); ei++)
  {
    i = get_id_from_vertex(*((*ei)->start_vertex));
    j = get_id_from_vertex(*((*ei)->end_vertex));
    set_distance(i, i, 0.0);
    set_distance(i, j, (*ei)->weight);
    set_predecessor(i, j, i);
  }
*/
}

rdgl_edge_weight_t 
AllPairsMap::get_distance(rdgl_all_pairs_map_id_t s,
                              rdgl_all_pairs_map_id_t v)
{
/*if (s < vertices->size() && v < vertices->size() && s >= 0 && v >= 0)
    return (*distances)[s][v];
  fprintf(stderr, "warning: out-of-range id (%d or %d / %d) in "
                  "AllPairsMap::get_distance\n", s, v, vertices->size());*/
  return (double) rdgl_ALL_PAIRS_INFINITY;
}
rdgl_edge_weight_t
AllPairsMap::d(rdgl_all_pairs_map_id_t s,
                   rdgl_all_pairs_map_id_t v) { return get_distance(s, v); }

rdgl_all_pairs_map_id_t
AllPairsMap::get_predecessor(rdgl_all_pairs_map_id_t s,
                                 rdgl_all_pairs_map_id_t v)
{
/*if (s < vertices->size() && v < vertices->size() && s >= 0 && v >= 0)
    return (*predecessors)[s][v];
  fprintf(stderr, "warning: out-of-range id (%d or %d / %d) in "
                  "AllPairsMap::get_predecessor\n", s, v, vertices->size());*/
  return rdgl_ALL_PAIRS_NULL;
}
rdgl_all_pairs_map_id_t
AllPairsMap::p(rdgl_all_pairs_map_id_t s,
                   rdgl_all_pairs_map_id_t v) { return get_predecessor(s, v); }
/*
rdgl_edge_weight_t
AllPairsMap::set_distance(rdgl_all_pairs_map_id_t s,
                              rdgl_all_pairs_map_id_t v,
                              rdgl_edge_weight_t w)
{
  //if ((rdgl_EXT2INT_VERTEX_ID(s) < vertices->size()) &&
  //    (rdgl_EXT2INT_VERTEX_ID(v) < vertices->size()))
  //  (*distances)[rdgl_EXT2INT_VERTEX_ID(s)][rdgl_EXT2INT_VERTEX_ID(v)] = w;
  if (s < vertices->size() && v < vertices->size() && s >= 0 && v >= 0)
    (*distances)[s][v] = w;
  return w;
}
*/

rdgl_edge_weight_t
AllPairsMap::set_distance(rdgl_all_pairs_map_id_t s,
                              rdgl_all_pairs_map_id_t v,
                              rdgl_edge_weight_t w)
{
  (*distances)[s][v] = w;
  return w;
}

rdgl_all_pairs_map_id_t
AllPairsMap::set_predecessor(rdgl_all_pairs_map_id_t s,
                                 rdgl_all_pairs_map_id_t v,
                                 rdgl_all_pairs_map_id_t p)
{
  /*(*predecessors)[s][v] = p;*/
  return p;
}

Path *
AllPairsMap::extract_path(Vertex & start, Vertex & end)
{
  /*
  std::vector<rdgl_all_pairs_map_id_t> vs;
  std::vector<rdgl_all_pairs_map_id_t>::iterator vi;
  rdgl_all_pairs_map_id_t s=0, e=0, p=0;
  Path * path = NULL;
  Vertex * u = NULL, * v = NULL;
  Edge * edge = NULL, search_edge;
  s = get_id_from_vertex(start);
  p = e = get_id_from_vertex(end);
  if (rdgl_ALL_PAIRS_IS_NULL(s) || rdgl_ALL_PAIRS_IS_NULL(e))
  {
    LOG_ERROR("failed to find endpoints in AllPairsMap::extract_path")
    goto errout;
  }
//std::cout << "looking for " << start << " - " << end << " path" << std::endl;
  for (p=e;
       (! rdgl_ALL_PAIRS_IS_NULL(p)) && (vs.size() < vertices->size());
       (p = get_predecessor(s, p)))
    vs.insert(vs.begin(), p);
  if (vs.size() > 1)
  {
    vi = vs.begin();
    if (! (u = get_vertex_from_id(*vi))) // grab the start vertex ptr
    {
      LOG_ERROR("failed to find vertex from vertex id")
      goto errout;
    }
    if ((u->distance != 0) || (u->predecessor != NULL))
    {
      vi++;
      path = new Path;
      while (vi != vs.end()) // while we're not to the last
      {
        if (! (v = get_vertex_from_id(*vi))) // grab the end vertex ptr
        {
          LOG_ERROR("failed to find vertex from vertex id")
          goto errout;
        }
        search_edge.start_vertex = u; // set up the search edge
        search_edge.end_vertex = v;
        if (! (edge = u->out_edges->find(search_edge))) // find it in the start
        {                                               // vertex's out edges
          LOG_ERROR("failed to find edge from vertex id")
          goto errout;
        }
//      std::cout << "edge: " << *edge;
        path->append(*edge); // append the next edge
        u = v; // set our end vertex to our start vertex
        vi++; // move to the next vertex in the path
      }
    }
  }
  return path;
  errout:
  if (path) delete path;
  path = NULL;
  return path;
  */
  return NULL;
}

#ifdef RUBY_H
VALUE
AllPairsMap::extract_ruby_path(Vertex & start, Vertex & end)
{
  VALUE v = Qnil;
  Path * p = extract_path(start, end);
  if (p)
    v = spath_to_rpath(*p);
  return v;
}
#endif

// you'd have to be insane to actually use this on anything but small graphs ...
PathSet *
AllPairsMap::extract_all_paths(void)
{
  PathSet * ps = new PathSet;
/*  Path * p = NULL;
  VertexList::iterator vi, vj;
  for (vi=vertices->begin(); vi!=vertices->end(); vi++)
    for (vj=vertices->begin(); vj!=vertices->end(); vj++)
      if (*vi != *vj)
        if ((p = extract_path(**vi, **vj)))
          ps->insert(*p);*/
  return ps;
}

#ifdef RUBY_H
VALUE
AllPairsMap::extract_all_ruby_paths(void)
{
  VALUE v = Qnil;
  PathSet * p = extract_all_paths();
  if (p)
    v = spaths_to_rpaths(*p);
  return v;
}
#endif

std::ostream &
operator<<(std::ostream & os, AllPairsMap & m)
{
  rdgl_all_pairs_map_id_t i, j;
  os << "Map:" << std::endl;
  for (i=0; i < m.vertices->size(); i++)
  {
    for (j=0; j < m.vertices->size(); j++)
    {
      os << "(d=" << m.d(i, j) << " p=" << m.p(i, j) << "), ";
    }
    os << std::endl;
  }
  return os;
}

/******************************************************************************/
/* Edge ***********************************************************************/
/******************************************************************************/

// Edge-based initializer for edge id's
/*void initialize_edge_id(rdgl_edge_id_t & _id,
                          Vertex & _s, Vertex & _d)
  {
    u64_to_u32_union * id = (u64_to_u32_union *) &_id,
                     * s  = (u64_to_u32_union *) &_s,
                     * d  = (u64_to_u32_union *) &_d;
#ifdef __LITTLE_ENDIAN__
    id->u32.upper = s->u32.upper; // the source entity
    id->u32.lower = d->u32.upper; // the destination entity
#elif __BIG_ENDIAN__
    id->u32.upper = s->u32.lower; // the source entity
    id->u32.lower = d->u32.lower; // the destination entity
#else
#error "BADNESS: no endianness defined!"
#endif
  }*/

// copy constructor
Edge::Edge(const Edge & e)
{
  initialize(e.start_vertex, e.end_vertex, e.weight, e.edge_id, NULL);
}

Edge::Edge(Vertex & s, Vertex & e,
                   rdgl_edge_weight_t w, rdgl_edge_id_t id, Graph * g)
{
  initialize(&s, &e, w, id, g);
}

Edge::Edge(Vertex * s, Vertex * e,
                   rdgl_edge_weight_t w, rdgl_edge_id_t id, Graph * g)
{
  initialize(s, e, w, id, g);
}

void
Edge::initialize(Vertex * s, Vertex * e,
                     rdgl_edge_weight_t w, rdgl_edge_id_t id, Graph * g)
{
  weight = w;
  start_vertex = s;
  end_vertex = e;
  if (g && s && e && id==0)
    g->initialize_edge_id(*s, *e, this->edge_id);
  else 
    edge_id = id;
#ifdef RUBY_H
  value = (VALUE) 0;
#endif
}

// destructor
Edge::~Edge(void) { ; }

// reset member data
void
Edge::reset(void)
{
  weight = 0.0;
  start_vertex = end_vertex = NULL;
}

// assignment operator
bool
Edge::operator=(const Edge & b)
{
  start_vertex = b.start_vertex;
  end_vertex = b.end_vertex;
  weight = b.weight;
  edge_id = b.edge_id;
}

// comparison operators
bool
Edge::operator<(const Edge & b) const
{
  return weight < b.weight;
}

bool
Edge::operator>(const Edge & b) const
{
  return weight > b.weight;
}

bool
Edge::operator==(const Edge & b) const
{
  //if (weight != b.weight) return false; // if the weights aren't equal, false
  if (! start_vertex || ! end_vertex || // if there are no vertices to compare
      ! b.start_vertex || ! b.end_vertex) return true;
  return ((*start_vertex == *(b.start_vertex)) && // if we can compare, do so
          (*end_vertex == *(b.end_vertex)) &&
          (edge_id == b.edge_id));
}

bool
Edge::operator!=(const Edge & b) const
{
  return (*this != b);
}

bool
rdgl_edge_lt_cmp(const Edge * a, const Edge * b)
{
  if (a->start_vertex->id < b->start_vertex->id) return true;
  if (a->start_vertex->id > b->start_vertex->id) return false;
  if (a->edge_id < b->edge_id) return true;
  if (a->edge_id > b->edge_id) return false;
  // NOTE: this really shouldn't happen -- it means someone has used a duplicate
  // edge_id from the same start vertex and asked us to compare the two
  if (vertex_id_lt_cmp(a->start_vertex, b->start_vertex)) return true;
  if (vertex_id_lt_cmp(b->start_vertex, a->start_vertex)) return false;
  return false;
}

// compares edges by using vertex start and end IDs (only)
bool
Edge::operator()(const Edge * a, Edge * b) const
{
  return rdgl_edge_lt_cmp(a, b);
  /*
  // this implements a sort of "less than" operator such that it forms an
  // ordering over all edges as required ...
  if ( (a->start_vertex) < (b->start_vertex) ) return true;
  if ( (a->start_vertex) > (b->start_vertex) ) return false;
  // now a->start == b->start, make sure the ends break an "unequal tie"
  if ( (a->end_vertex)   < (b->end_vertex)   ) return true;
  //if ( *(a->end_vertex) > *(b->end_vertex)   )
  return false;
  */
}

bool
Edge::relax(void)
{
  bool relaxed = false;
  Vertex * u = this->start_vertex,
             * v = this->end_vertex;
  if (! u || ! v)
  {
    std::cerr << "warning: null vertex detected on an edge in relax"
              << std::endl;
    return relaxed;
  }
  if (v->distance > u->distance + this->weight)
  {
    v->distance = u->distance + this->weight;
    //u->forward_edge = this;
    v->back_edge = this;
    relaxed = true;
  }
  return relaxed;
}

std::ostream &
operator<<(std::ostream & os, Edge & e)
{
  os << "[ "
     << *(e.start_vertex)
     << " - (" << (int) e.edge_id << "; " << e.weight << ") -> "
     << *(e.end_vertex)
     << " ]" << std::endl;
  return os;
}

/******************************************************************************/
/* Path ***********************************************************************/
/******************************************************************************/

void
Path::append(Edge & e)
{
  push_back(&e);
}

void
Path::prepend(Edge & e)
{
  insert(begin(), &e);
}

std::ostream &
operator<<(std::ostream & os, Path & p)
{
  Path::iterator i;
  os << "edges (" << p.size() << "): " << std::endl;
  for (i = p.begin(); i != p.end(); i++)
    os << **i << " ";
  return os;
}

bool
Path::operator==(Path & p)
{
  Path::iterator i, j;
  for (i = this->begin(), j = p.begin();
       i != this->end() && j != p.end(); i++, j++)
    if (**i != **j) return false;
  return true;
}

/******************************************************************************/
/* PathSet ********************************************************************/
/******************************************************************************/

void
PathSet::append(Path & p)
{
  push_back(&p);
}

void
PathSet::insert(Path & p)
{
  std::vector<Path *>::insert(begin(), &p);
}

void
PathSet::prepend(Path & p)
{
  std::vector<Path *>::insert(begin(), &p);
}

std::ostream &
operator<<(std::ostream & os, PathSet & p)
{
  PathSet::iterator i;
  os << "paths: " << std::endl;
  for (i = p.begin(); i != p.end(); i++)
    os << **i << std::endl;
  return os;
}

/******************************************************************************/
/* Graph **********************************************************************/
/******************************************************************************/

// null constructor
Graph::Graph(void)
{
  vertices = new VertexSet;
  edges = new EdgeSet;
  file_name = NULL;
  sp_algorithm = NULL;
#ifdef RUBY_H
  value = (VALUE) 0;
#endif
}

// (shallow) copy constructor
Graph::Graph(Graph & g)
{
  vertices = g.vertices;
  edges = g.edges;
  file_name = g.file_name;
  sp_algorithm = g.sp_algorithm;
#ifdef RUBY_H
  value = (VALUE) 0;
#endif
}

// destructor
Graph::~Graph(void)
{
// NOTE: if we're using Ruby, Edge and Vertex objects are all added
//       from within Ruby and thus Ruby GC is tasked with their dellocation
#ifndef RUBY_H
  if (vertices)
  {
    VertexSet::iterator vi;
    for (vi=vertices->begin(); vi!=vertices->end(); vi++)
      delete *vi;
    vertices->clear();
    delete vertices;
    vertices = NULL;
  }
  if (edges)
  {
    EdgeSet::iterator ei;
    for (ei=edges->begin(); ei!=edges->end(); ei++)
      delete *ei;
    edges->clear();
    delete edges;
    edges = NULL;
  }
#endif
  file_name = NULL;
  sp_algorithm = NULL;
}

size_t
Graph::num_vertices(void)
{
  if (vertices) return vertices->size();
  return 0;
}

size_t
Graph::order(void)
{
  return num_vertices();
}

size_t
Graph::num_edges(void)
{
  if (vertices) return edges->size();
  return 0;
}

size_t
Graph::size(void)
{
  return num_edges();
}

// resetting edge weights and vertex id's makes no sense for an
// entire graph (you should just destroy it and read in another)
// so this just resets vertex predecessors and distances (for use
// with another iteration of bellman-ford / dijkstra / etc.
void
Graph::reset(Vertex * start)
{
  if (! vertices) return;
  VertexSet::iterator v;
  for (v=vertices->begin(); v != vertices->end(); v++)
  {
    (*v)->forward_edge = NULL;
    (*v)->back_edge = NULL;
    if (start && (**v == *start))
      (*v)->distance = 0.0;
    else (*v)->distance = MAX_DISTANCE;
  }
}

void
Graph::reset(void)
{
  this->reset((Vertex *) NULL);
}

void
Graph::reset(Vertex & start)
{
  this->reset(&start);
}

void
Graph::initialize_edge_id(Vertex & s, Vertex & d,
                              rdgl_edge_id_t & edge_id)
{
  std::vector<rdgl_edge_id_t> edge_ids;
  EdgeSet::iterator ei;
  edge_id = 1;
  if (this->edges)
    for (ei = this->edges->begin() ; ei != this->edges->end() ; ei++)
      if ((*(*ei)->start_vertex == s))
        edge_ids.push_back((*ei)->edge_id);
  sort(edge_ids.begin(), edge_ids.end());
  for (rdgl_edge_id_t i=0; i<edge_ids.size(); i++)
    if (edge_id == edge_ids[i]) edge_id++;
}

#ifdef RUBY_H
VALUE Graph::set_edge_id(Edge & edge)
#else
void Graph::set_edge_id(Edge & edge)
#endif
{
  EdgeSet::iterator ei;
  bool found = false;
  if (this->edges)
    for (ei = this->edges->begin() ; ei != this->edges->end() ; ei++)
    {
/*    std::cerr << "comparing (" << *(*ei)->start_vertex << ") with ("
                << *edge.start_vertex << ") and " << std::endl
                << "comparing (" << *(*ei)->end_vertex << ") with ("
                << *edge.end_vertex << ")" << std::endl;*/
      if ((*(*ei)->start_vertex == *edge.start_vertex) &&
          (*(*ei)->end_vertex == *edge.end_vertex))
      {
        //std::cerr << "they're equal!" << std::endl;
        found = true;
        edge.edge_id = (*ei)->edge_id;
        break;
      }
      //else std::cerr << "they're NOT equal!" << std::endl;
    }
  if (! found)
    initialize_edge_id(*edge.start_vertex, *edge.end_vertex, edge.edge_id);
  //std::cerr << "set_edge_id picked " << (int) edge.edge_id << std::endl;
#ifdef RUBY_H
  return LONG2NUM(edge.edge_id);
#endif
}

// Dijkstra's (original) algorithm
bool
Graph::run_traditional_dijkstra(Vertex & start)
{
  if (! edges || ! vertices) return false;
  VertexList * vertex_list = new VertexList;
  Vertex * u = NULL, * v = NULL;
  VertexSet::iterator si;
  EdgeSet::iterator ei;
  reset(start);
  for (si = vertices->begin(); si != vertices->end(); si++)
    vertex_list->insert(**si);
  while (vertex_list->size() > 0)
  {
    u = vertex_list->remove_smallest();
    for (ei = u->out_edges->begin(); ei != u->out_edges->end(); ei++)
      (*ei)->relax();
  }
  vertex_list->clear();
  delete vertex_list;
  this->sp_algorithm = SG_SP_ALGO_TRAD_DIJKSTRA;
#ifdef DEBUG
  std::cerr << "Post-Dijsktra Graph " << std::endl << *this;
#endif
  return true;
}

// Dijkstra's (poor STL-heap-based) algorithm
bool
Graph::run_stl_heap_dijkstra(Vertex & start)
{
  bool relaxed = false;
  if (! edges || ! vertices) return false;
  VertexHeap * vertex_heap = new VertexHeap;
  Vertex * u = NULL, * v = NULL;
  VertexSet::iterator si;
  EdgeSet::iterator ei;
  reset(start);
  for (si =  vertices->begin(); si != vertices->end(); si++)
    vertex_heap->insert(**si);
  while (vertex_heap->size() > 0)
  {
    relaxed = false;
    u = vertex_heap->pop();
    for (ei = u->out_edges->begin(); ei != u->out_edges->end(); ei++)
      relaxed = ((*ei)->relax() || relaxed);
    // we may have just changed the minimum weight edge; reheapify
    if (relaxed) vertex_heap->reheapify();
  }
  vertex_heap->clear();
  delete vertex_heap;
  this->sp_algorithm = SG_SP_ALGO_DIJKSTRA;
  return true;
}

// Dijkstra's (poor STL-heap-based) algorithm
bool
Graph::run_priority_queue_dijkstra(Vertex & start)
{
/*      vi D(N, 987654321); 
      // distance from start vertex to each vertex

      priority_queue<ii,vector<ii>, greater<ii> > Q; 
      // priority_queue with reverse comparison operator, 
      // so top() will return the least distance
      // initialize the start vertex, suppose it’s zero
      D[0] = 0;
      Q.push(ii(0,0));

      // iterate while queue is not empty
      while(!Q.empty()) {

            // fetch the nearest element
            ii top = Q.top();
            Q.pop();
                        
            // v is vertex index, d is the distance
            int v = top.second, d = top.first;

            // this check is very important
            // we analyze each vertex only once
            // the other occurrences of it on queue (added earlier) 
            // will have greater distance
            if(d <= D[v]) {
                  // iterate through all outcoming edges from v
                  tr(G[v], it) {
                        int v2 = it->first, cost = it->second;
                        if(D[v2] > D[v] + cost) {
                              // update distance if possible
                              D[v2] = D[v] + cost;
                              // add the vertex to queue
                              Q.push(ii(D[v2], v2));

                        }
                  }
            }
      }
  priority_queue<Vertex *,
                 vector<Vertex *>, deref_greater<Vertex *> > Q;
  Q.push(start);
  while(! Q.empty())
  {
    Vertex top = Q.top();
    Q.pop();

    if ((top.id == start.id) || 
*/
/*  bool relaxed = false;
  if (! edges || ! vertices) return false;
  VertexHeap * vertex_heap = new VertexHeap;
  Vertex * u = NULL, * v = NULL;
  VertexSet::iterator si;
  EdgeSet::iterator ei;
  reset(start);
  for (si =  vertices->begin(); si != vertices->end(); si++)
    vertex_heap->insert(**si);
  while (vertex_heap->size() > 0)
  {
    relaxed = false;
    u = vertex_heap->pop();
    for (ei = u->out_edges->begin(); ei != u->out_edges->end(); ei++)
      relaxed = ((*ei)->relax() || relaxed);
    // we may have just changed the minimum weight edge; reheapify
    if (relaxed) vertex_heap->reheapify();
  }
  vertex_heap->clear();
  delete vertex_heap;
  this->sp_algorithm = SG_SP_ALGO_DIJKSTRA;
  return true;*/
}

// Wrapper
bool
Graph::run_dijkstra(Vertex & s) { run_traditional_dijkstra(s); }

// Bellman-Ford's algorithm
bool
Graph::run_bellman_ford(Vertex & start)
{
  if (! edges || ! vertices) return false;
  Vertex * u = NULL, * v = NULL;
  Edge * e = NULL;
  EdgeSet::iterator ei;
  reset(start);
  for (int i=0; i<vertices->size()-1; i++)
  {
    for (ei = edges->begin(); ei != edges->end(); ei++)
      (*ei)->relax();
  }
  for (ei = edges->begin(); ei != edges->end(); ei++)
  {
    e = *ei;
    u = e->start_vertex;
    v = e->end_vertex;
    if (v->distance > u->distance + e->weight)
      return false;
  }
  this->sp_algorithm = SG_SP_ALGO_BELLMAN_FORD;
  return true;
}

// Floyd-Warshall
AllPairsMap *
Graph::run_floyd_warshall(void)
{
  rdgl_all_pairs_map_id_t i, j, k;
  rdgl_edge_weight_t n;
  VertexList * vs = NULL;
  VertexList::iterator vi, vj, vk;
  AllPairsMap * m = new AllPairsMap(*this);
/*if (! m || ! m->distances) goto out;
  vs = m->vertices;
  for (vk = vs->begin(), k=0; vk != vs->end(); vk++, k++)
    for (vi = vs->begin(), i=0; vi != vs->end(); vi++, i++)
      for (vj = vs->begin(), j=0; vj != vs->end(); vj++, j++)
        if (m->d(i, j) > (n = m->d(i, k) + m->d(k, j)))
        {
          m->set_distance(i, j, n);
          m->set_predecessor(i, j, k);
        }
  out:*/
  return m;
}

#ifdef RUBY_H
VALUE
Graph::run_ruby_floyd_warshall(void)
{
  AllPairsMap * m = NULL;
  if ((m = run_floyd_warshall()))
    return m->value;
  rb_raise(rb_eStandardError, "run_floyd_warshall failed");
}
#endif

// Yen
PathSet *
Graph::run_yen(Vertex & s, Vertex & t) { ; }

// extract a path to the given vertex from the best paths already calculated
// using bellman-ford or dijkstra
Path *
Graph::extract_path(Vertex & end)
{
  Edge * e = NULL;
  Vertex * v = NULL, * lastv = NULL;
  int traversals = 0;
  Path * p = new Path;
  v = this->vertices->find(end);
  if (! v)
  {
    std::cerr << "given end vertex " << end
              << " not found in extract_path" << std::endl;
    std::cerr << "graph looks like this " << *this;
    return NULL;
  }
#ifdef DEBUG
  std::cerr << "in extract path starting with end vertex: " << *v << std::endl;
#endif
  lastv = v;
  do
  {
    if (v->back_edge)
    {
      p->prepend(*(v->back_edge));
      v = v->back_edge->start_vertex;
      lastv = v;
      traversals++;
    }
    else v = NULL;
  } while (v && (traversals < this->num_edges()));
  if ((lastv->distance != 0) || (lastv->back_edge != NULL))
  {
#ifdef DEBUG
    std::cerr << "Bad path in extract paths (last vertex not source!)"
              << "; lastv: " << lastv
              << "; p->length: " << p->length()
              << "; lastv->distance: " << lastv->distance
              << "; lastv->back_edge: " << lastv->back_edge
              << std::endl;    
#endif
    delete p;
    p = NULL;
  }
#ifdef DEBUG
  else std::cerr << "Computed path: " << std::endl << *p;
#endif
  return p;
}

#ifdef RUBY_H
VALUE
Graph::extract_ruby_path(Vertex & end)
{
  VALUE v = Qnil;
  Path * p = extract_path(end);
  if (p)
    v = spath_to_rpath(*p);
  return v;
}
#endif

PathSet *
Graph::extract_all_paths(void)
{
  VertexSet::iterator i;
  Path * p = NULL;
  PathSet * set = new PathSet;
  for (i=this->vertices->begin(); i!=this->vertices->end(); i++)
  {
    p = this->extract_path(**i);
    set->insert(*p);
  }
  return set;
}

Edge *
Graph::add_edge(rdgl_vertex_id_t v1, rdgl_vertex_id_t v2,
                    rdgl_edge_weight_t w, rdgl_edge_id_t edge_id)
{
  Edge * e = NULL, * added_edge = NULL;
  int id[2] = { v1, v2 };
  Vertex * v[2] = { NULL, NULL };
  // create two vertices
  for (int i=0; i<2; i++) v[i] = new Vertex(id[i]);
  // create an edge
  e = new Edge((v[0]), (v[1]), w, edge_id, NULL);
  // try to add it
  added_edge = add_edge(*e);
  // if the start vertex was already there, delete it
  if ((void *) added_edge->start_vertex != (void *) e->start_vertex)
  {
    delete e->start_vertex;
    e->start_vertex = added_edge->start_vertex;
  }
  // if the end vertex was already there, delete it
  if ((void *) added_edge->end_vertex != (void *) e->end_vertex)
  {
    delete e->end_vertex;
    e->end_vertex = added_edge->end_vertex;
  }
  // if the edge already there, delete it
  if ((void *) added_edge != (void *) e)
  {
    delete e;
    e = added_edge;
  }
  return e;
}

// add_edge:
// (1) tries to find the given edge's vertex endpoints in vertices
// (2) adds them if it does not
// (3) RESETS them (start_vertex, end_vertex) if it DOES
// (4) tries to find the given edge in edges
// (5) adds it if it does not
// (6) RETURNS the LOCATED EDGE if it does
// Therefore, callers should check to see if what they passed was
// passed back; if not, they'll want to deallocate.
Edge *
Graph::add_edge(Edge & er)
{
  Edge * e = &er;
  Vertex * v[2] = { e->start_vertex, e->end_vertex },
             * v_in_set = NULL;
  for (int i=0; i<2; i++)
  {
    // try to add the vertex
    v_in_set = add_vertex(*v[i]);
    // if the vertex we "added" was not already there (i.e.,
    // if our pointer was added and returned, just use it
    // OTHERWISE, reset the given pointer to the new one
    if (((void *) v_in_set) != ((void *) v[i]))
    {
      if (i==0) e->start_vertex = v_in_set;
      else      e->end_vertex = v_in_set;
    }
  }
  // (4) try to find the given edge
  Edge * e_in_set = edges->find(*e);
  // (5) add it if we don't find it
  if (! e_in_set)
    edges->insert(*e);
  // (6) reset our pointer to the new edge if we do
  else
    e = e_in_set;
  // since we're now added, add an out edge from us
  e->start_vertex->add_out_edge(*e);
  return e;
}

Edge *
Graph::random_edge(void)
{
  return edges->random();
}

Edge *
Graph::remove_edge(Edge & e)
{
  Edge * re = NULL;
  if ((re = edges->remove(e)))
    if (e.start_vertex && e.start_vertex->out_edges)
      e.start_vertex->out_edges->remove(e);
    else
      std::cerr << "null pointer in Graph::remove_edge" << std::endl;
  return re;
}

// see if the vertex is already there
Vertex *
Graph::add_vertex(Vertex & vr)
{
  Vertex * v = &vr,
             * v_in_set = NULL;
  v_in_set = vertices->find(*v);
  if (! v_in_set)
    vertices->insert(*v);
  else
    v = v_in_set;
  return v;
}

Vertex *
Graph::random_vertex(void)
{
  return vertices->random();
}

Vertex *
Graph::remove_vertex(Vertex & v)
{
  return vertices->remove(v);
}

#ifdef RUBY_H
void
Graph::ruby_gc_mark(void)
{
  VertexSet::iterator vi;
  EdgeSet::iterator ei;
  if (this->edges)
    for (ei = this->edges->begin() ; ei != this->edges->end() ; ei++)
      if ((*ei)->value) rb_gc_mark((*ei)->value);
  if (this->vertices)
    for (vi = this->vertices->begin() ; vi != this->vertices->end() ; vi++)
      if ((*vi)->value) rb_gc_mark((*vi)->value);
}
#endif

std::ostream &
operator<<(std::ostream & os, Graph & g)
{
  if (g.file_name) os << "source file: " << g.file_name << std::endl;
  if (g.sp_algorithm) os << "shortest path algorithm: "
                         << g.sp_algorithm << std::endl;
  os << "vertices:" << std::endl;
  os << *(g.vertices) << std::endl;
  os << "edges:" << std::endl;
  os << *(g.edges);
}

static int
open_file(const char * file_name)
{
  int fd = 0;
  if (! file_name)
  {
    fprintf(stderr, "NULL file name\n");
    return -1;
  }
  // open the file
  if      (*file_name == '-') fd = STDIN;
  else if (! (fd = open(file_name, O_RDONLY, "r")))
  {
    fprintf(stderr, "failed to open file!\n");
    return -1;
  }
  return fd;
}

static const char *
mmap_fd(int fd)
{
  // file decl
  size_t file_size = 0;
  struct stat file_info;
  char * file_data = NULL;
  if (fstat(fd, &file_info))
  {
    fprintf(stderr, "failed to stat file!\n");
    return NULL;
  }
  file_size = file_info.st_size;
  file_data = (char *) mmap(NULL, file_size, PROT_READ, MAP_SHARED, fd, 0);
  if (file_data == MAP_FAILED)
  {
    perror("mmap failed");
    return NULL;
  }
  return file_data;
}

/* inet graph files have the following format:
 
 * generate the output of graph in the format   *
 * 3 sections: the first section 1 line gives   *
 * the number of nodes and the number of links. *
 * the second section gives the list of nodes   *
 * and their x and y coordinates.  The third    *
 * section is the adjacency list with each link *
 * listed only once, i.e. symmetric links       *
 *                                              *
 * nodes links                                  *
 * id   x_cord  y_cord                          *
 * ...                                          *
 * id   id      cost                            *
 * ...                                          *

 */
bool
Graph::from_inet_file(const char * file_name)
{
  bool retval = false;
  FILE * f = NULL;
  int fd = -1;
  int num_nodes=0, num_edges=0, s=0, t=0, cost=0;
  int i=0;
  // open the fd
  if ((fd = open_file(file_name)) < 0)
  {
    fprintf(stderr, "failed to open file!\n");
    goto out;
  }
  if (! (f = fdopen(fd, "r")))
  {
    fprintf(stderr, "failed to open file!\n");
    goto out;
  }
  // grab the first line: number nodes/edges
  if (fscanf(f, "%d %d", &num_nodes, &num_edges) != 2)
  {
    fprintf(stderr, "failed to read initial line!\n");
    goto out;
  }
  if (num_nodes == 0)
  {
    fprintf(stderr, "parsed no nodes!\n");
    goto out;
  }
  // skip the next section (location)
  for (i=0; i<num_nodes; i++)
    if (fscanf(f, "%d %d %d", &s, &t, &cost) != 3)
    {
      fprintf(stderr, "failed to read second section!\n");
      goto out;
    }
  // now grab the edges
  while (fscanf(f, "%d %d %d", &s, &t, &cost) == 3)
  {
    add_edge(s, t, (double) cost);
    add_edge(t, s, (double) cost);
  }
  this->file_name = file_name;
  retval = true;
out:
  if (f) fclose(f);
  if (fd > 2) close(fd);
  return retval;
}

bool
Graph::from_skitter_file(const char * file_name)
{
  bool retval = false;
  FILE * f = NULL;
  int fd = -1;
  int num_nodes=0, num_edges=0, s=0, t=0, cost=1;
  int i=0;
  // open the fd
  if ((fd = open_file(file_name)) < 0)
  {
    fprintf(stderr, "failed to open file!\n");
    goto out;
  }
  if (! (f = fdopen(fd, "r")))
  {
    fprintf(stderr, "failed to open file!\n");
    goto out;
  }
  // grab the edges
  while (fscanf(f, "%d %d", &s, &t) == 2)
  {
    add_edge(s, t, (double) cost);
    add_edge(t, s, (double) cost);
  }
  this->file_name = file_name;
  retval = true;
out:
  if (f) fclose(f);
  if (fd > 2) close(fd);
  return retval;
}

#define INET_SUFFIX ".inet"
#define SKITTER_SUFFIX ".net"
bool
Graph::from_file(const char * file_name)
{
  size_t len = strlen(file_name);
  bool skitter_file = false;
  if (len == 0)
  {
    fprintf(stderr, "bad file name\n");
    return false;
  }
  if (len > strlen(INET_SUFFIX))
    if (strncmp(&file_name[len - strlen(INET_SUFFIX)], 
                INET_SUFFIX,
                strlen(INET_SUFFIX)) == 0)
      skitter_file = false;
  else if (len > strlen(SKITTER_SUFFIX))
    if (strncmp(&file_name[len - strlen(SKITTER_SUFFIX)], 
                SKITTER_SUFFIX,
                strlen(SKITTER_SUFFIX)) == 0)
      skitter_file = true;
  else
  {
    fprintf(stderr, "unrecognized file type: %s\n", file_name);
    return false;
  }
  if (skitter_file)
    return from_skitter_file(file_name);
  else
    return from_inet_file(file_name);
}
#undef INET_SUFFIX
#undef SKITTER_SUFFIX

}

#if 0
#define NMATCH 5
bool
Graph::from_file(const char * file_name)
{
  bool retval = false, do_regex_free = false;
  // file stuff
  int fd = -1;
  size_t file_size = 0;
  struct stat file_info;
  const char * file_data_start = NULL;
  char * file_data = NULL;
  // regex stuff
  char errbuf[255];
  int regex_out = 0, offset = 0;
  regex_t preg;
  regmatch_t pmatch[NMATCH];
  const char * match_str = "([[:digit:]]+)[[:space:]]+([[:digit:]]+)([[:space:]]+([[:digit:]]+(\\.[[:digit:]]+)?)?)?$";
  // converted values
  int s = 0, t = 0;
  rdgl_edge_weight_t v = 0.0;
  // open the fd
  if ((fd = open_file(file_name)) < 0)
  {
    fprintf(stderr, "failed to open file!\n");
    goto out;
  }
  if (fstat(fd, &file_info))
  {
    fprintf(stderr, "failed to stat file!\n");
    goto out;
  }
  file_size = file_info.st_size;
  // mmap the fd
  file_data_start = mmap_fd(fd);
  file_data = (char *) file_data_start;
  if (! file_data)
  {
    fprintf(stderr, "failed to mmap file descriptor\n");
    goto out;
  }
  // set up the regex
  regex_out = regcomp(&preg, match_str, REG_EXTENDED | REG_NEWLINE);
  if (regex_out != 0)
  {
    errbuf[0] = '\0';
    regerror(regex_out, &preg, errbuf, 255);
    fprintf(stderr, "regcomp said %s\n", errbuf);
    goto out;
  }
  else do_regex_free = true;
  // get ready to read data out of the mmap
  while (offset < file_size)
  {
    regex_out = regexec(&preg, file_data, NMATCH, pmatch, REG_EXTENDED);
    if (regex_out != REG_NOMATCH)
    {
      if (pmatch[0].rm_so != -1 && pmatch[2].rm_so != -1)
      {
        s = strtol(&file_data[pmatch[0].rm_so], NULL, 10);
        t = strtol(&file_data[pmatch[2].rm_so], NULL, 10);
        if (pmatch[4].rm_so != -1)
          v = strtod(&file_data[pmatch[4].rm_so], NULL);
        else
          v = 1.0;
        add_edge(s, t, v);
      }
      else fprintf(stderr, "matched but no match was recorded ... huh?\n");
    }
    else 
    {
      errbuf[0] = '\0';
      regerror(regex_out, &preg, errbuf, 255);
      fprintf(stderr, "match failed; regex said %s\n", errbuf);
    }
    do { offset++; } while ((*(file_data_start+offset-1) != '\n') && 
                            (offset < file_size)); // advance to next newline+1
    file_data = (char *) file_data_start + offset;
  }
  retval = true;
  this->file_name = (char *) file_name;
  out:
  if (do_regex_free) regfree(&preg);
  if (file_data_start) munmap((char *) file_data_start, file_size);
  if (fd > 2) close(fd);
  return retval;
}
#undef NMATCH
#endif

#if 0
int main(int argc, const char * argv[])
{
  Graph * g = NULL;
  Path * p1 = NULL, * p2 = NULL;
  VertexSet::iterator i, j;
  if (argc < 2)
  {
    fprintf(stderr, "need an arg\n");
    return 255;
  }
  g = new Graph;
  if (! g->from_file(argv[1]))
  {
    std::cerr << "badness reading from graph file!" << std::endl;
    return 255;
  }
  // pick two vertices
  Vertex * v1 = NULL, * v2 = NULL;
  v1 = g->vertices->random();
  v2 = g->vertices->random();
  //Vertex vr1(2133), vr2(865);
  //v1 = &vr1;  v2 = &vr2;
  //std::cout << "Pre-run, G is " << *g;
  g->run_dijkstra(*v1);
  std::cerr << "used v1: " << *v1 << " v2: " << *v2 << std::endl;
  //std::cout << "Post-Dijkstra G is " << *g;
  p1 = g->extract_path(*v2);
  //std::cout << "Dijkstra v1-v2 path: " << std::endl << *p << std::endl;
  g->run_bellman_ford(*v1);
  //std::cout << "Post-Bellman G is " << *g;
  std::cerr << "used v1: " << *v1 << " v2: " << *v2 << std::endl;
  p2 = g->extract_path(*v2);
  //std::cout << "Bellman-Ford v1-v2 path: " << std::endl << *p << std::endl;
  //std::cerr << *p1 << " -> " << *p2 << " ";
  if (*p1 == *p2) std::cerr << "paths match" << std::endl;
  else std::cerr << "paths differ!" << std::endl
                 << "Dijkstra: " << std::endl
                 << *p1 << std::endl
                 << "Bellman-Ford: " << std::endl
                 << *p2 << std::endl;
  delete g;
  return 0;
}
#endif

