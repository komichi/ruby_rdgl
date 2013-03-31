#!/usr/bin/env ruby

require 'rubygems'
require 'rdgl'
require 'rdgl/disjoint_dijkstra'

file_name = ARGV.shift
if ! file_name
  $stderr.puts "usage: #{$0} <graph file>"
  exit 1
end

g = Rdgl::Graph.new

# reads a skitter file; returns two vertices from it (first, last)
def read_skitter_file(g, file)
  line_num = 0
  vx1 = vx2 = nil
  File.open(file) { |f|
    w = 1.0
    f.each { |l|
      l =~ /(\d+)\s+(\d+)\s+(\d+(\.\d+)?)?/
      v1_id = $1.to_i
      v2_id = $2.to_i
      w = $3 ? $3.to_f : 1.0
      unless v1_id && v2_id
        $stderr.puts("failed to parse line #{l}")
        next
      end
      v1 = Rdgl::Vertex.new(v1_id, Rdgl::INFINITY)
      vx1 = v1 if (line_num == 0) 
      vx2 = v2 = Rdgl::Vertex.new(v2_id, Rdgl::INFINITY)
      e1 = Rdgl::Edge.new(v1, v2, w)
      e2 = Rdgl::Edge.new(v2, v1, w)
      g.set_edge_id(e1)
      g.add_edge(e1)
      g.set_edge_id(e2)
      g.add_edge(e2)
      line_num += 1
    }
  }
  return [vx1, vx2]
end

#def run_disjoint_dijkstra(g, firstv, endv)
#  paths = Rdgl::PathSet.new
#  g.run_dijkstra(firstv)
#  while (new_path = g.extract_path(endv))
#    paths.add_path(new_path)
#    (new_path.size-1).times { |i|
#      e = Edge.new(new_path[i], new_path[i+1], 1.0)
#      g.remove_edge(e)
#    }
#    g.run_dijkstra(firstv)
#  end
#  return paths
#end

start_time = Time.now
v1, v2 = read_skitter_file(g, file_name)
end_time = Time.now
puts "Read required: " + (end_time - start_time).to_s + " seconds"
puts "Graph size/order: " + g.size.to_s + "/" + g.order.to_s

# Run Dijkstra
start_time = Time.now
g.run_dijkstra(v1)
end_time = Time.now
puts "Dijkstra (#{v1.to_s} - #{v2.to_s}) required: " + (end_time - start_time).to_s + " seconds"

p = g.extract_path(v2)
puts p.to_s

# Run the disjoint Dijsktra thing
start_time = Time.now
paths = g.run_disjoint_dijkstra(v1, v2)
end_time = Time.now
puts "Disjoint Dijkstra required: " + (end_time - start_time).to_s + " seconds"
puts "Disjoint (#{v1.to_s} - #{v2.to_s}) paths (" + paths.size.to_s + "):"
puts paths.to_s

# Run Bellman-Ford
start_time = Time.now
g.run_bellman_ford(v1)
end_time = Time.now
puts "Bellman-Ford (#{v1.to_s} - #{v2.to_s}) required: " + (end_time - start_time).to_s + " seconds"
p = g.extract_path(v2)
puts p.to_s

if false

if g.size < 100
  # Run the Floyd-Warshall algorithm
  start_time = Time.now
  map = g.run_floyd_warshall
  end_time = Time.now
  puts map.to_s
  puts "--"
  paths = map.extract_all_paths
  puts "Floyd-Warshall required: " + (end_time - start_time).to_s + " seconds"
  puts "Floyd-Warshall (#{v1.to_s} - #{v2.to_s}) paths (" + paths.size.to_s + "):"
  puts paths.to_s
end

end

