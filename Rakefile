require 'rubygems'
require "rubygems/package_task"

spec = Gem::Specification.new do |s| 
  s.name = 'rdgl'
  s.version = '0.0.1'
  s.author = 'John Lane'
  s.email = 'johnrlane+rdgl @nospam@ gmail.com'
  s.homepage = 'http://github.com/komichi/ruby_rdgl'
  s.platform = Gem::Platform::RUBY
  s.summary = 'Ruby Really Dumb Graph Library'
  s.description = 'A very basic (but fast) graph library for unidirectional weighted (or unweighted) graphs'
  s.files = FileList['lib/rdgl/*', 'ext/**/*', 'examples/*', 'test/*'].to_a
  s.require_path = 'lib'
  s.bindir = 'bin' # Use these for applications.
  s.executables = [ ]
#  s.default_executable = ''
#  s.autorequire = ''
#  s.test_files = FileList['{test}/**/*test.rb'].to_a
  s.has_rdoc = true
  s.extra_rdoc_files = ['README', 'LICENSE']
  s.extensions << 'ext/rdgl/extconf.rb'
end

Gem::PackageTask.new(spec) do |pkg|
  pkg.gem_spec = spec
  pkg.need_tar = false
end

desc 'Does a full compile, test run'
task :default => [:compile, :package]

desc 'Compiles all extensions'
task :compile do
end

task :package => "pkg/#{spec.name}-#{spec.version}.gem" do
  puts 'generated latest version'
end

