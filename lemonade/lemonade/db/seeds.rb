# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ name: 'Chicago' }, { name: 'Copenhagen' }])
#   Mayor.create(name: 'Emanuel', city: cities.first)
#Dataflow.delete_all

%w{elasticsearch.rb filter.rb input.rb output.rb}.each do |file|
  require "#{Rails.root}/db/#{file}"
end

#csv input
pop_input
pop_elastic
pop_filter
pop_output
# elasticsearch
# input

default_dataflow = {
  name: 'default_dataflow',
  class_id: 'xxxx',
  student_id: 'xxxxx',
  interface: {'cells':[]},
  log: {test: 'xxx'}
}
Dataflow.create(default_dataflow)
