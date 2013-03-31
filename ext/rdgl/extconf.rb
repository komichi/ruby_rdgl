require 'mkmf'

is_win32 = (/mswin32|mingw|cygwin|bccwin32/ =~ RUBY_PLATFORM)
is_macosx = (/darwin/ =~ RUBY_PLATFORM)
is_linux = (/linux/ =~ RUBY_PLATFORM)

puts "this is " + RUBY_PLATFORM

if is_macosx
  $CPPFLAGS += ' -DIS_MACOSX'
elsif is_linux
  $CPPFLAGS += ' -DIS_LINUX'
elsif is_win32
  $CPPFLAGS += ' -DIS_WIN32'
end

$CPPFLAGS += ([1].pack("I") == [1].pack("N")) ? " -D__BIG_ENDIAN__" :
                                                " -D__LITTLE_ENDIAN__"

have_library('stdc++')
create_makefile("rdgl")

