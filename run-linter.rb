#!/usr/bin/env ruby
#Easy, clean interface for linting all cpp files in the project.
#Just run `ruby run-linter.rb` and optionally direct its output to
#a log file using `ruby run-linter.rb > linter-output.txt`
source_files = ["src/main.cpp", "src/actor.cpp", "src/display.cpp",
                "src/gameboard.cpp", "src/input.cpp", "src/actor.cpp",
                "src/item.cpp"]
linter_flags = ["cppcoreguidelines-*", "bugprone-*", "performance-*",
                "clang-analyzer-*", "portability-*", "readability-*", "misc-*"]
source_files.each do |file|
  puts "\t" + file.upcase
  warnings = `clang-tidy --checks=#{linter_flags.join(",")} #{file} \
           -- std=c++11 #{file}`.split("\n")
  warnings.select!{|line| line.include?(": warning: ")}
  warnings.each do |warning|
    puts warning
    puts "\n\n"
  end
  puts "---------------\n\n"
end
