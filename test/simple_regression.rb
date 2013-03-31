#!/usr/bin/env ruby

require 'rubygems'
require 'rdgl'

# testing

# utility vertices
v1 = Rdgl::Vertex.new(999, 1.1)
vx1 = Rdgl::Vertex.new(999, 41.1)
v2 = Rdgl::Vertex.new(9999, 2.2)
vx2 = Rdgl::Vertex.new(9999, 42.2)

# utility edges
e = Rdgl::Edge.new(v1, v2, 92.2)
ex = Rdgl::Edge.new(vx1, vx2, 942.2)

puts "e.edge_id = " + e.edge_id.to_s
puts "ex.edge_id = " + ex.edge_id.to_s

# does Rdgl::Graph remove edges based on vertex id's alone?
g = Rdgl::Graph.new
g.set_edge_id(e)
g.add_edge(e)
g.set_edge_id(ex)
puts "new e.edge_id = " + e.edge_id.to_s
puts "new ex.edge_id = " + ex.edge_id.to_s
num_edges = g.num_edges
g.remove_edge(ex)
if g.num_edges != (num_edges-1)
  $stderr.puts "PROBLEM: removing edges based on vertex id's alone"
else
  $stderr.puts "OK: removing edges based on vertex id's alone"
end
g = nil

# does Rdgl::Graph allow multiple vertex insertion with same id
g = Rdgl::Graph.new
g.add_vertex(v1)
g.add_vertex(v2)
g.add_vertex(vx1)
g.add_vertex(vx2)
if g.num_vertices != 2
  $stderr.puts "PROBLEM: allows multiple vertex insertion with same id"
else
  $stderr.puts "OK: does not allow multiple vertex insertion with same id"
end
g = nil

# does Rdgl::Graph remove vertices based on vertex id alone
g = Rdgl::Graph.new
g.add_vertex(v1)
g.add_vertex(v2)
g.remove_vertex(vx1)
g.remove_vertex(vx2)
if g.num_vertices != 0
  $stderr.puts "PROBLEM: removing vertices based on vertex id alone"
else
  $stderr.puts "OK: removing vertices based on vertex id alone"
end
g = nil

# does Rdgl::Graph random vertex/edge return a valid vertex when size = 0, 1, 2?
g = Rdgl::Graph.new
vs = ve = nil
if (vs = g.random_vertex)
  $stderr.puts "PROBLEM: returning random vertex when order = 0"
else
  $stderr.puts "OK: returning random vertex when order = 0"
end
if (ve = g.random_edge)
  $stderr.puts "PROBLEM: returning random edge when size = 0"
else
  $stderr.puts "OK: returning random edge when size = 0"
end
g.add_vertex(v1)
unless g.random_vertex
  $stderr.puts "PROBLEM: returning random vertex when order = 1"
else
  $stderr.puts "OK: returning random vertex when order = 1"
end
g.add_edge(e)
unless (ve = g.random_edge)
  $stderr.puts "PROBLEM: returning random vertex when size = 2"
else
  $stderr.puts "OK: returning random edge when size = 2"
  #$stderr.puts ve.to_s
end

