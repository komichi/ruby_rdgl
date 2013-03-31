#!/usr/bin/env ruby

require 'rdgl'

module Rdgl

  class Graph

    # reads a skitter file; returns two vertices from it (first, last)
    def read_skitter_file(file)
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
          v1 = Vertex.new(v1_id, Rdgl::INFINITY)
          vx1 = v1 if (line_num == 0) 
          vx2 = v2 = Vertex.new(v2_id, Rdgl::INFINITY)
          e1 = Edge.new(v1, v2, w, self)
          e2 = Edge.new(v2, v1, w, self)
          add_edge(e1)
          add_edge(e2)
          line_num += 1
        }
      }
      return [vx1, vx2]
    end

    def run_disjoint_dijkstra(firstv, endv, k=0)
      removed_edges = [ ]
      paths = PathSet.new
      run_dijkstra(firstv)
      while (new_path = extract_path(endv)) && ((k==0) || (paths.size < k))
        paths.add_path(new_path)
        break if (firstv == endv) # handle the case where there are no edges
        #$stderr.puts "graph before removing edges: " + to_s
        new_path.each { |edge| removed_edges.push remove_edge(edge) }
        #$stderr.puts "graph after removing edges: " + to_s
        run_dijkstra(firstv)
      end
      # put the removed edges back
      removed_edges.each { |edge| add_edge(edge) }
      return paths
    end

  end # class Graph

end # module Rdgl

if $0 == __FILE__

  g = Rdgl::Graph.new

  start_time = Time.now
  v1, v2 = g.read_skitter_file(ARGV[0])
  end_time = Time.now
  puts "Read required: " + (end_time - start_time).to_s + " seconds"
  puts "Graph size/order: " + g.size.to_s + "/" + g.order.to_s

  # Run Dijkstra
  start_time = Time.now
  g.run_dijkstra(v1)
  end_time = Time.now
  puts "Dijkstra required: " + (end_time - start_time).to_s + " seconds"
  p = g.extract_path(v2)
  puts p.to_s

  # Run Bellman-Ford
  start_time = Time.now
  g.run_bellman_ford(v1)
  end_time = Time.now
  puts "Bellman-Ford required: " + (end_time - start_time).to_s + " seconds"
  p = g.extract_path(v2)
  puts p.to_s

  # Run the disjoint Dijsktra thing
  start_time = Time.now
  paths = g.run_disjoint_dijkstra(v1, v2)
  end_time = Time.now
  puts "Disjoint Dijkstra required: " + (end_time - start_time).to_s +
       " seconds"
  puts "Disjoint paths (" + paths.size.to_s + "):"
  puts paths.to_s

  if g.size < 100
    # Run the Floyd-Warshall algorithm
    start_time = Time.now
    map = g.run_floyd_warshall
    end_time = Time.now
    puts map.to_s
    puts "--"
    paths = map.extract_all_paths
    puts "Floyd-Warshall required: " + (end_time - start_time).to_s + " seconds"
    puts "Floyd-Warshall paths (" + paths.size.to_s + "):"
    puts paths.to_s
  end

end

